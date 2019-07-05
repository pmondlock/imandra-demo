type encoding =
  | Chunked
  | Fixed of int64
  | Unknown
type chunk =
  | Chunk of string
  | Final_chunk of string
  | Done
let string_of_encoding =
  function
  | Chunked  -> "chunked"
  | Fixed i -> Printf.sprintf "fixed[%Ld]" i
  | Unknown  -> "unknown"
let has_body =
  function
  | Fixed 0L -> `No
  | Chunked |Fixed _ -> `Yes
  | Unknown  -> `Unknown
