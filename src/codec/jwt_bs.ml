(* Auto-generated from "jwt.atd" *)
              [@@@ocaml.warning "-27-32-35-39"]
open Jwt_t

let write__3 = (
  Atdgen_codec_runtime.Encode.option_as_constr (
    Atdgen_codec_runtime.Encode.string
  )
)
let read__3 = (
  Atdgen_codec_runtime.Decode.option_as_constr (
    Atdgen_codec_runtime.Decode.string
  )
)
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
let write__2 = (
  Atdgen_codec_runtime.Encode.option_as_constr (
    write__1
  )
)
let read__2 = (
  Atdgen_codec_runtime.Decode.option_as_constr (
    read__1
  )
)
let write_accessToken = (
  Atdgen_codec_runtime.Encode.make (fun (t : accessToken) ->
    (
    Atdgen_codec_runtime.Encode.obj
      [
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.int
            )
          ~name:"exp"
          t.exp
        ;
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.int
            )
          ~name:"iat"
          t.iat
        ;
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"sub"
          t.sub
        ;
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"iss"
          t.iss
        ;
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"aud"
          t.aud
        ;
          Atdgen_codec_runtime.Encode.field
            (
            write__1
            )
          ~name:"scp"
          t.scp
        ;
          Atdgen_codec_runtime.Encode.field_o
            (
            write__1
            )
          ~name:"groups"
          t.groups
        ;
          Atdgen_codec_runtime.Encode.field_o
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"cid"
          t.cid
        ;
          Atdgen_codec_runtime.Encode.field_o
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"uid"
          t.uid
        ;
          Atdgen_codec_runtime.Encode.field_o
            (
            Atdgen_codec_runtime.Encode.string
            )
          ~name:"jti"
          t.jti
        ;
          Atdgen_codec_runtime.Encode.field
            (
            Atdgen_codec_runtime.Encode.int
            )
          ~name:"ver"
          t.ver
      ]
    )
  )
)
let read_accessToken = (
  Atdgen_codec_runtime.Decode.make (fun json ->
    (
      ({
          exp =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.int
              |> Atdgen_codec_runtime.Decode.field "exp"
            ) json;
          iat =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.int
              |> Atdgen_codec_runtime.Decode.field "iat"
            ) json;
          sub =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.field "sub"
            ) json;
          iss =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.field "iss"
            ) json;
          aud =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.field "aud"
            ) json;
          scp =
            Atdgen_codec_runtime.Decode.decode
            (
              read__1
              |> Atdgen_codec_runtime.Decode.field "scp"
            ) json;
          groups =
            Atdgen_codec_runtime.Decode.decode
            (
              read__1
              |> Atdgen_codec_runtime.Decode.fieldOptional "groups"
            ) json;
          cid =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.fieldOptional "cid"
            ) json;
          uid =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.fieldOptional "uid"
            ) json;
          jti =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.string
              |> Atdgen_codec_runtime.Decode.fieldOptional "jti"
            ) json;
          ver =
            Atdgen_codec_runtime.Decode.decode
            (
              Atdgen_codec_runtime.Decode.int
              |> Atdgen_codec_runtime.Decode.field "ver"
            ) json;
      } : accessToken)
    )
  )
)
