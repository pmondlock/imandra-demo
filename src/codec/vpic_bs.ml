(* Auto-generated from "vpic.atd" *)
              [@@@ocaml.warning "-27-32-35-39"]
open Vpic_t

let write_make_entry = (
  Atdgen_codec_runtime.Encode.make (fun (t : make_entry) ->
    (
    Atdgen_codec_runtime.Encode.obj
      [
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.int
            )
          ~name:"Make_ID"
          t.id
        ;
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"Make_Name"
          t.name
      ]
    )
  )
)
let read_make_entry = (
  Atdgen_codec_runtime.Decode.make (fun json ->
    (
      ({
          id =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.int
              |> Atdgen_codec_runtime.Decode.field "Make_ID"
            ) json;
          name =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.field "Make_Name"
            ) json;
      } : make_entry)
    )
  )
)
let write__1 = (
  Atdgen_codec_runtime.Encode.list (
    write_make_entry
  )
)
let read__1 = (
  Atdgen_codec_runtime.Decode.list (
    read_make_entry
  )
)
let write_make_response = (
  Atdgen_codec_runtime.Encode.make (fun (t : make_response) ->
    (
    Atdgen_codec_runtime.Encode.obj
      [
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.int
            )
          ~name:"Count"
          t.count
        ;
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"Message"
          t.message
        ;
          Atdgen_codec_runtime.Encode.field
            (
            write__1
            )
          ~name:"Results"
          t.results
      ]
    )
  )
)
let read_make_response = (
  Atdgen_codec_runtime.Decode.make (fun json ->
    (
      ({
          count =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.int
              |> Atdgen_codec_runtime.Decode.field "Count"
            ) json;
          message =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.field "Message"
            ) json;
          results =
            Atdgen_codec_runtime.Decode.decode
            (
              read__1
              |> Atdgen_codec_runtime.Decode.field "Results"
            ) json;
      } : make_response)
    )
  )
)
