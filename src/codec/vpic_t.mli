(* Auto-generated from "vpic.atd" *)
              [@@@ocaml.warning "-27-32-35-39"]

type make_entry = { id: int; name: string }

type make_response = {
  count: int;
  message: string;
  results: make_entry list
}
