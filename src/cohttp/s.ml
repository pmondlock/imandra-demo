module type Request  =
  sig
    type t =
      {
      headers: Header.t[@ocaml.doc " HTTP request headers "];
      meth: Code.meth[@ocaml.doc " HTTP request method "];
      resource: string[@ocaml.doc " Request path and query "];
      version: Code.version[@ocaml.doc " HTTP version, usually 1.1 "];
      encoding:
        Transfer.encoding[@ocaml.doc
                           " transfer encoding of this HTTP request "];}
    [@@deriving (fields, sexp)]
    val make :
      ?meth:Code.meth ->
        ?version:Code.version ->
          ?encoding:Transfer.encoding -> ?headers:Header.t -> Uri.t -> t
    [@@ocaml.doc " Return true whether the connection should be reused "]
    val is_keep_alive : t -> bool[@@ocaml.doc
                                   " Return true whether the connection should be reused "]
    val uri : t -> Uri.t
    val make_for_client :
      ?headers:Header.t ->
        ?chunked:bool -> ?body_length:int64 -> Code.meth -> Uri.t -> t
  end[@@ocaml.doc " Module type signatures for Cohttp components "]
module type Response  =
  sig
    type t =
      {
      encoding:
        Transfer.encoding[@ocaml.doc
                           " Transfer encoding of this HTTP response "];
      headers: Header.t[@ocaml.doc " response HTTP headers "];
      version: Code.version[@ocaml.doc " (** HTTP version, usually 1.1 *) "];
      status:
        Code.status_code[@ocaml.doc " HTTP status code of the response "];
      flush: bool;}[@@deriving (fields, sexp)]
    val make :
      ?version:Code.version ->
        ?status:Code.status_code ->
          ?flush:bool ->
            ?encoding:Transfer.encoding -> ?headers:Header.t -> unit -> t
  end
module type Body  =
  sig
    type t
    val to_string : t -> string
    val to_string_list : t -> string list
    val empty : t
    val is_empty : t -> bool
    val of_string : string -> t
    val of_string_list : string list -> t
    val map : (string -> string) -> t -> t
    val transfer_encoding : t -> Transfer.encoding
  end
