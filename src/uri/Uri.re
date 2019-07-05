module Url = {
  type protocol = [ | `HTTP | `HTTPS];

  type t = {
    protocol,
    port: option(int),
    query: option(list((string, list(string)))),
    host: option(string),
    path: string,
  };

  module SearchParams = {
    type t;
    [@bs.send] external append: (t, string, string) => t = "append";
  };

  type url;

  [@bs.module "url"] [@bs.val] external url: url = "URL";
  [@bs.module "url"] external url_of_string: string => t = "parse";
  [@bs.module "url"] external searchParams: unit => SearchParams.t = "URLSearchParams";
  [@bs.send] external hash: t => string = "hash";
  [@bs.send] [@bs.return null_undefined_to_opt] external host: t => option(string) = "host";
  [@bs.send] [@bs.return null_undefined_to_opt] external password: t => option(string) = "password";
  [@bs.send] external pathname: t => string = "pathname";
  [@bs.send] external port: t => string = "port";
  [@bs.send] external protocol: t => string = "protocol";
  [@bs.send] external username: t => string = "username";
  [@bs.send] external toString: t => string = "format";
  [@bs.send] external searchParams: t => SearchParams.t = "searchParams";

  [@bs.send] external encodeURIComponent: string => string = "encodeURIComponent";

  let string_of_protocol = p =>
    switch (p) {
    | `HTTP => "http"
    | `HTTPS => "https"
    };

  let string_of_params = (ps: list((string, list(string)))): string => {
    let spread_ps = (k, vs) =>
      List.fold_right((v, a) => a ++ "&" ++ encodeURIComponent(k) ++ "=" ++ encodeURIComponent(v), vs, "");
    List.fold_right(((k, vs), a) => a ++ "&" ++ spread_ps(k, vs), ps, "");
  };

  let init = (): t => {protocol: `HTTP, path: "/", host: None, port: None, query: None};

  let to_string = (uri: t): string => {
    let proto = string_of_protocol(uri.protocol);
    let portString =
      switch (uri.port) {
      | Some(p) => {j|:$p|j}
      | None => ""
      };
    let search =
      switch (uri.query) {
      | Some(ps) => "?" ++ string_of_params(ps)
      | None => ""
      };
    let host = uri.host->Belt.Option.getWithDefault("");
    let path = uri.path;
    Printf.sprintf("%s://%s%s%s", proto, host, path, search);
  };
};

type t = Url.t;

type component = [
  | `Scheme
  | `Authority
  | `Userinfo
  | `Host
  | `Path
  | `Query
  | `Query_key
  | `Query_value
  | `Fragment
];

let empty = Url.init();

let compare = (a: t, b: t) =>
  switch (a.host, b.host) {
  | (None, Some(_)) => 1
  | (Some(_), None) => (-1)
  | (Some(hostA), Some(hostB)) =>
    switch (String.compare(hostA, hostB)) {
    | 0 => String.compare(a.path, b.path)
    | v => v
    }
  | _ => 0
  };

let equal = (a, b) => compare(a, b) == 0;

let scheme_of_string = scheme =>
  switch (String.lowercase(scheme)) {
  | "http" => `HTTP
  | "https" => `HTTPS
  | _ => `HTTP
  };

let pct_encode = (~scheme=?, ~component=?, s) => {
  let scheme' = scheme->Belt.Option.getWithDefault("http");
  let component' = component->Belt.Option.getWithDefault(`Path);
  "";
};

let pct_decode = s => "";

let of_string = s => Url.url_of_string(s);

let to_string = uri => Url.toString(uri);

let resolve = (s, a, b) => a;

let canonicalize = uri => uri;

let make = (~scheme=?, ~userinfo=?, ~host=?, ~port=?, ~path=?, ~query=?, ~fragment=?, unit): t => {
  protocol: scheme->Belt.Option.mapWithDefault(`HTTP, scheme_of_string),
  host,
  port,
  path: path->Belt.Option.getWithDefault("/"),
  query,
};

let with_uri = (~scheme=?, ~userinfo=?, ~host=?, ~port=?, ~path=?, ~query=?, ~fragment=?, uri) => uri;

let query = uri => [];

let verbatim_query = uri => None;

let encoded_of_query = (~scheme=?, params) => "";

let query_of_encoded = query_string => [];

let with_query = (uri, params) => uri;

let with_query' = (uri, params) => uri;

let get_query_param' = (uri, param_name) => None;

let get_query_param = (uri, param_name) => None;

let add_query_param = (uri, params) => uri;

let add_query_param' = (uri, params) => uri;

let add_query_params = (uri, params) => uri;

let add_query_params' = (uri, params) => uri;

let remove_query_param = (uri, param_name) => uri;

let path = uri => Url.(uri.path);

let path_and_query = uri => "";

let with_path = (uri, path) => uri;

let scheme = uri => Some(Url.(string_of_protocol(uri.protocol)));

let with_scheme = (uri, scheme) =>
  Url.{...uri, protocol: scheme->Belt.Option.mapWithDefault(`HTTP, scheme_of_string)};

let userinfo = uri => None;

let with_userinfo = (uri, user_info) => uri;

let user = uri => None;

let password = uri => None;

let with_password = (uri, password) => uri;

let host = uri => Url.(uri.host);

let with_host = (uri, host) => uri;

let host_with_default = (~default=?, uri) =>
  Url.(
    switch (uri.host, default) {
    | (Some(h), _) => h
    | (None, Some(h)) => h
    | _ => ""
    }
  );

let port = uri => Url.(uri.port);

let with_port = (uri, port) => uri;

let fragment = uri => None;

let with_fragment = (uri, fragment) => uri;

let pp_hum = (fmt, uri) => ();
