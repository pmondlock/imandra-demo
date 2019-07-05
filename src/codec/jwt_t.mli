(* Auto-generated from "jwt.atd" *)
              [@@@ocaml.warning "-27-32-35-39"]

type accessToken = {
  exp: int;
  iat: int;
  sub: string;
  iss: string;
  aud: string;
  scp: string list;
  groups: string list option;
  cid: string option;
  uid: string option;
  jti: string option;
  ver: int
}
