let _ = [%bs.raw {| (!Buffer) ? require('buffer') : null |}]
type buffer
type encoding =
  | Base64
  | Ascii
  | Utf8
  | Binary
  | Hex
module Buffer =
  struct
    type t = buffer
    external of_string : string -> string -> t = "from"[@@bs.val
                                                         "Buffer.from"]
    external to_string : t -> string = "toString"[@@bs.send ]
    external to_encoding : t -> string -> string = "toString"[@@bs.send ]
  end
let string_of_encoding e =
  match e with
  | Base64  -> "base64"
  | Ascii  -> "ascii"
  | Utf8  -> "utf8"
  | Binary  -> "binary"
  | Hex  -> "hex"
let encode s = Buffer.of_string s (string_of_encoding Utf8)
let decode b = Buffer.to_string b
let decode_for_encoding b enc = Buffer.to_encoding b (string_of_encoding enc)
