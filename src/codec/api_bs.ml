(* Auto-generated from "api.atd" *)
              [@@@ocaml.warning "-27-32-35-39"]
open Api_t

let write__1 = (
  Atdgen_codec_runtime.Encode.list (
    Atdgen_codec_runtime.Encode.string
  )
)
let read__1 = (
  Atdgen_codec_runtime.Decode.list (
    Atdgen_codec_runtime.Decode.string
  )
)
let write_make_response = (
  Atdgen_codec_runtime.Encode.make (fun (t : make_response) ->
    (
    Atdgen_codec_runtime.Encode.obj
      [
          Atdgen_codec_runtime.Encode.field
            (
            write__1
            )
          ~name:"results"
          t.results
      ]
    )
  )
)
let read_make_response = (
  Atdgen_codec_runtime.Decode.make (fun json ->
    (
      ({
          results =
            Atdgen_codec_runtime.Decode.decode
            (
              read__1
              |> Atdgen_codec_runtime.Decode.field "results"
            ) json;
      } : make_response)
    )
  )
)
