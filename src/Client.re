module Result = Belt.Result;
module Option = Belt.Option;

[%raw "require('isomorphic-fetch')"];

module JsHeader = {
  [@bs.send]
  external forEach: (Fetch.Headers.t, (string, string) => unit) => unit =
    "forEach";
};

module Response = {
  type t = {
    encoding: Transfer.encoding,
    version: Code.version,
    status: Code.status_code,
    headers: Header.t,
  };

  let status = res => res.status;

  let headers = res => res.headers;

  let encoding = res => res.encoding;

  let version = res => res.version;

  let make = (~encoding=?, ~version=?, ~status=?, ~headers=?, ()): t => {
    encoding: encoding->Option.getWithDefault(Transfer.Unknown),
    version: version->Option.getWithDefault(`HTTP_1_1),
    status: status->Option.getWithDefault(`OK),
    headers: headers->Option.getWithDefault(Header.init()),
  };
};

type error = [
  | `JsonDecodeError(string)
  | `UnhandledIOError
  | `HttpError(Response.t, Body.t)
  | `BodyError(string)
];

let json_of_string = s =>
  switch (Js.Json.parseExn(s)) {
  | exception ex =>
    Result.Error(`JsonDecodeError("Unable to parse response as JSON"))
  | json => Result.Ok(json)
  };

let json_of_body = (body: Body.t) =>
  switch (body) {
  | `Empty => Result.Ok(Js.Json.null)
  | `String(s) => json_of_string(s)
  | `Strings(chunks) => String.concat("", chunks)->json_of_string
  };

let body_of_json = (json: Js.Json.t): Body.t =>
  Js.Json.stringify(json) |> Body.of_string;

module Request = {
  type t = {
    meth: Code.meth,
    version: Code.version,
    encoding: Transfer.encoding,
    headers: Header.t,
    uri: Uri.t,
  };

  let make = (~meth=`GET, ~version=`HTTP_1_1, ~encoding=?, ~headers=?, uri) => {
    meth,
    version,
    encoding:
      switch (encoding) {
      | None => Transfer.Unknown
      | Some(enc) => enc
      },
    headers: headers->Option.getWithDefault(Header.init()),
    uri,
  };
};

let makeHeaders = (headers: option(Header.t)) => {
  let headers' =
    headers->Option.map(headers => {
      let dict = Js.Dict.empty();
      Header.iter((k, v) => Js.Dict.set(dict, k, List.hd(v)), headers);
      dict;
    });
  switch (headers') {
  | Some(h) => Fetch.HeadersInit.makeWithDict(h)
  | None => Fetch.HeadersInit.makeWithArray([||])
  };
};

let mapHeaders = (headers: Fetch.Headers.t): Header.t => {
  let hs = ref(Header.init());
  headers->JsHeader.forEach((v, k) => hs := (hs^)->Header.add(k, v));
  hs^;
};

let mapFetchResponse =
    (res: Fetch.Response.t)
    : Future.t(Result.t((Response.t, Body.t), [> error])) => {
  let headers = Fetch.Response.headers(res);

  let contentLength = (headers: Fetch.Headers.t): option(int64) =>
    Fetch.Headers.get("content-length", headers)
    ->Option.map(len => Int64.of_string(len));

  let encoding =
    switch (
      Fetch.Headers.get("transfer-encoding", headers)
      ->Option.flatMap(h => Stringext.find_from(h, ~pattern="chunked"))
    ) {
    | None =>
      contentLength(headers)
      ->Option.map(len => Transfer.Fixed(len))
      ->Option.getWithDefault(Transfer.Unknown)
    | Some(_) => Transfer.Chunked
    };

  FutureJs.fromPromise(
    Fetch.Response.text(res),
    err => {
      Js.Console.error @@ err;
      `UnhandledIOError;
    },
  )
  ->Future.mapOk(bodyText => {
      let status = Fetch.Response.status(res);

      let response =
        Response.make(
          ~version=`HTTP_1_1,
          ~encoding,
          ~status=Code.status_of_code(status),
          ~headers=mapHeaders(headers),
          (),
        );

      let body =
        switch (bodyText) {
        | "" => `Empty
        | str => Body.of_string(str)
        };
      (response, body);
    });
};

let fetchInit = (~uri: Uri.t, ~meth, ~body=?, ~headers=?, ()) => {
  let headers = makeHeaders(headers);
  switch (body) {
  | Some(body) =>
    Fetch.fetchWithInit(
      Uri.to_string(uri),
      Fetch.RequestInit.make(
        ~method_=meth,
        ~body=Fetch.BodyInit.make(body),
        ~headers,
        (),
      ),
    )
  | None =>
    Fetch.fetchWithInit(
      Uri.to_string(uri),
      Fetch.RequestInit.make(~method_=meth, ~headers, ()),
    )
  };
};

let fetch = (jsFetch: Js.Promise.t('a)) =>
  FutureJs.fromPromise(jsFetch, err => `UnhandledIOError);

let string_of_method = method_ =>
  switch (method_) {
  | Fetch.Get => "GET"
  | Post => "POST"
  | Put => "PUT"
  | Head => "HEAD"
  | Delete => "DELETE"
  | Connect => "CONNECT"
  | Options => "OPTIONS"
  | Trace => "TRACE"
  | Patch => "PATCH"
  | Other(s) => s
  };

let canRetry = code => {
  let validCodes = [429, 408];
  List.mem(code, validCodes);
};

let responseHandler = (delayedFetch, dwell, rlt) =>
  switch (rlt) {
  | Result.Ok(res) =>
    res |> Fetch.Response.ok ?
      rlt->Future.value :
      res
      ->mapFetchResponse
      ->Future.flatMapOk(((res, body)) =>
          Result.Error(`HttpError((res, body)))->Future.value
        )
  | _ => rlt->Future.value
  };

let fetchWithRetry = (~uri, ~meth, ~body=?, ~headers=?, ()) => {
  let timing = [500, 1000];
  let delayedFetch = _ =>
    fetch(fetchInit(~uri, ~meth, ~body?, ~headers?, ()));

  List.fold_left(
    (rlt, dwell) =>
      rlt->Future.flatMap(responseHandler(delayedFetch, dwell)),
    delayedFetch(),
    timing,
  )
  ->Future.map(rlt =>
      switch (rlt) {
      | Ok(res) =>
        if (res |> Fetch.Response.ok) {
          rlt;
        } else {
          Error(`RetryExhausted(res));
        }
      | Error(_) => rlt
      }
    );
};

let mapMethToFetchMethod = (meth: Code.meth) =>
  switch (meth) {
  | `GET => Fetch.Get
  | `POST => Post
  | `HEAD => Head
  | `DELETE => Delete
  | `PATCH => Patch
  | `PUT => Put
  | `OPTIONS => Options
  | `TRACE => Trace
  | `CONNECT => Connect
  | `Other(s) => Other(s)
  };

let call =
    (
      ~headers: option(Header.t)=?,
      ~body: option(Body.t)=?,
      meth: Code.meth,
      uri: Uri.t,
    )
    : Future.t(Result.t((Response.t, Body.t), [> error])) => {
  let method_string = Code.string_of_method(meth);

  let headers =
    Option.mapWithDefault(
      headers, Header.init_with("content-type", "application/json"), hs =>
      Header.add_unless_exists(hs, "content-type", "application/json")
    );

  fetchWithRetry(
    ~uri,
    ~meth=mapMethToFetchMethod(meth),
    ~headers,
    ~body=?Option.map(body, Body.to_string),
    (),
  )
  ->Future.flatMap(rlt =>
      switch (rlt) {
      | Ok(res) => mapFetchResponse(res)
      | Error(`RetryExhausted(res)) =>
        mapFetchResponse(res)
        ->Future.map(rlt =>
            switch (rlt) {
            | Ok(a) => Result.Error(`HttpError(a))
            | Error(a) => Result.Error(a)
            }
          )
      | Error(
          `JsonDecodeError(_) | `UnhandledIOError | `HttpError(_, _) |
          `BodyError(_),
        ) as e =>
        e->Future.value
      }
    );
};

let get =
    (~headers: option(Header.t)=?, uri: Uri.t)
    : Future.t(Result.t((Response.t, Body.t), [> error])) =>
  call(~headers?, `GET, uri);

let post =
    (~body: option(Body.t)=?, ~headers: option(Header.t)=?, uri: Uri.t)
    : Future.t(Result.t((Response.t, Body.t), [> error])) =>
  call(~headers?, ~body?, `POST, uri);

let delete =
    (~body: option(Body.t)=?, ~headers: option(Header.t)=?, uri: Uri.t)
    : Future.t(Result.t((Response.t, Body.t), [> error])) =>
  call(~headers?, ~body?, `DELETE, uri);

let put =
    (~body: option(Body.t)=?, ~headers: option(Header.t)=?, uri: Uri.t)
    : Future.t(Result.t((Response.t, Body.t), [> error])) =>
  call(~headers?, ~body?, `PUT, uri);

let patch =
    (~body: option(Body.t)=?, ~headers: option(Header.t)=?, uri: Uri.t)
    : Future.t(Result.t((Response.t, Body.t), [> error])) =>
  call(~headers?, ~body?, `PATCH, uri);

let json_of_res = ((_res, body)) => body |> json_of_body |> Future.value;
