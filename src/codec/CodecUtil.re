/* [decode reader json] will take a given Js.Json.t [json] and decode it according to [reader].*/
let decode = (reader, json) =>
  switch (json |> Atdgen_codec_runtime.Decode.decode(reader)) {
  | exception (Json_decode.DecodeError(err)) =>
    Belt.Result.Error(`DecodeError(err))
  | res => Belt.Result.Ok(res)
  };

/* [encode] encodes a JSON into a string */
let encode = Atdgen_codec_runtime.Encode.encode;

/* [decode_body reader req] decodes the body of an Express.Request [req] as a JSON structure into a struct
   for an ATD codec with the reader [reader] */
let decode_body = (reader, req) =>
  switch (req |> Express.Request.bodyJSON) {
  | None =>
    Belt.Result.Error(`ClientError("HTTP Body Expected but None Found"))
  | Some(json) => decode(reader, json)
  };
