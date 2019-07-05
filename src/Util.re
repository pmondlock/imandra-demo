module Result = Belt.Result;
module Option = Belt.Option;

module Infix = {
  let (>>) = (f, g, x) => x |> f |> g;

  let (+<$>) = Future.mapOk;
  let (+>>=) = Future.flatMapOk;
  let (+|-) = Future.tapOk;

  let (@<$>) = Future.map;
  let (@>>=) = Future.flatMap;
  let (@|-) = Future.tap;

  let (>>=) = Belt.Result.flatMap;
  let (<$>) = Belt.Result.map;
};

module Auth = {
  type t = Jwt_t.accessToken;

  type error = [
    | `Unknown
    | `ExpiredToken
    | `MalformedToken
    | `InvalidSignature
  ];

  /**
  [verify ~jwt ~key] currently does not perform token verification. Returns [Error(`Unknown)]
  */
  let verify = (~jwt: string, ~key: bytes) => Result.Error(`Unknown);

  /**
  [decode ~jwt] takes a standard RFC-7519 encoded access token and decodes it to a [Token_t.accessToken].
  Returns [Error(`MalformedToken)] if it cannot find, decode, or parse the token from [jwt].
  */
  let decode = (~jwt: string) =>
    switch (Js.String.split(".", jwt)) {
    | [|hdr, payload, sig_|] =>
      let buf = B64.Buffer.of_string(payload, "base64");
      let b64 = B64.(decode_for_encoding(buf, Utf8));
      let json =
        try (Js.Json.parseExn(b64)->Result.Ok) {
        | _ => Error(`MalformedToken)
        };
      json->Result.flatMap(jwt =>
        switch (
          jwt |> Atdgen_codec_runtime.Decode.decode(Jwt_bs.read_accessToken)
        ) {
        | exception _ => Error(`MalformedToken)
        | token => Ok(token)
        }
      );
    | _ => Error(`MalformedToken)
    };

  /**
  [groups token] retrieves the "groups" list from the token
  */
  let groups = token => Jwt_t.(token.groups)->Option.getWithDefault([]);

  let stripAuthHeader = hdr => Js.String.replace("bearer ", "", hdr);

  let findRole = (hdr, roleName) => {
    switch (decode(~jwt=stripAuthHeader(hdr))) {
    | Ok(token) =>
      if (List.mem(roleName, groups(token))) {
        Result.Ok(token);
      } else {
        Result.Error(`Unauthorized);
      }
    | Error(err) =>
      Js.Console.log2("JWT Malformed", err);
      Result.Error(`Unauthorized);
    };
  };

  let requireRole = (roleName, req) =>
    switch (Express.Request.get("authorization", req)) {
    | Some(token) => findRole(token, roleName)
    | None => Result.Error(`Unauthorized)
    };
};

let ok = x => Belt.Result.Ok(x);
let error = x => Belt.Result.Error(x);

let get =
    (
      path: string,
      handler:
        (Express.Request.t, Express.Response.t) => Future.t(Express.complete),
      app,
    ) => {
  Express.App.get(app, ~path) @@
  Express.PromiseMiddleware.from((_next, req, res) =>
    Js.Promise.make((~resolve, ~reject as _) =>
      handler(req, res)->Future.get(r' => resolve(. r'))
    )
  );
  app;
};
