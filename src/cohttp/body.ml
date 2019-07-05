type t = [ `Empty  | `String of string  | `Strings of string list ]
let empty = `Empty
let is_empty =
  function
  | `Empty|`String ""|`Strings [] -> true
  | `String _|`Strings _ -> false
let to_string =
  function
  | `Empty -> ""
  | `String s -> s
  | `Strings sl -> String.concat "" sl
let to_string_list =
  function | `Empty -> [] | `String s -> [s] | `Strings sl -> sl
let of_string s = `String s
let of_string_list s = `Strings s
let transfer_encoding =
  function
  | `Empty -> Transfer.Fixed 0L
  | `String s -> Transfer.Fixed (Int64.of_int (String.length s))
  | `Strings _ -> Transfer.Chunked
let length =
  function
  | `Empty -> 0L
  | `String s -> Int64.of_int (String.length s)
  | `Strings sl ->
      sl |>
        (List.fold_left
           (fun a  ->
              fun b  ->
                ((b |> String.length) |> Int64.of_int) |> (Int64.add a)) 0L)
let map f =
  function
  | `Empty -> `Empty
  | `String s -> `String (f s)
  | `Strings sl -> `Strings (List.map f sl)
