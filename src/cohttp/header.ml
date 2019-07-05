module LString :
  sig
    type t
    val of_string : string -> t
    val to_string : t -> string
    val compare : t -> t -> int
  end =
  struct
    type t = string
    let of_string x = String.lowercase x
    let to_string x = x
    let compare a b = String.compare a b
  end
module StringMap = Map.Make(LString)
type t = string list StringMap.t
let user_agent = Printf.sprintf "ocaml-cohttp/%s" Conf.version
let compare = StringMap.compare Pervasives.compare
let headers_with_list_values =
  Array.map LString.of_string
    [|"accept";"accept-charset";"accept-encoding";"accept-language";"accept-ranges";"allow";"cache-control";"connection";"content-encoding";"content-language";"expect";"if-match";"if-none-match";"link";"pragma";"proxy-authenticate";"te";"trailer";"transfer-encoding";"upgrade";"vary";"via";"warning";"www-authenticate"|]
let is_header_with_list_value =
  let tbl = Hashtbl.create (Array.length headers_with_list_values) in
  headers_with_list_values |> (Array.iter (fun h  -> Hashtbl.add tbl h ()));
  (fun h  -> Hashtbl.mem tbl h)
let init () = StringMap.empty
let is_empty x = StringMap.is_empty x
let init_with k v = StringMap.singleton (LString.of_string k) [v]
let add h k v =
  let k = LString.of_string k in
  try StringMap.add k (v :: (StringMap.find k h)) h
  with | Not_found  -> StringMap.add k [v] h
let add_list h l = List.fold_left (fun h  -> fun (k,v)  -> add h k v) h l
let add_multi h k l = List.fold_left (fun h  -> fun v  -> add h k v) h l
let add_opt h k v =
  match h with | None  -> init_with k v | Some h -> add h k v
let remove h k = let k = LString.of_string k in StringMap.remove k h
let replace h k v = let k = LString.of_string k in StringMap.add k [v] h
let get h k =
  let k = LString.of_string k in
  try
    let v = StringMap.find k h in
    if is_header_with_list_value k
    then Some (String.concat "," v)
    else Some (List.hd v)
  with | Not_found |Failure _ -> None
let mem h k = StringMap.mem (LString.of_string k) h
let add_unless_exists h k v = if mem h k then h else add h k v
let add_opt_unless_exists h k v =
  match h with | None  -> init_with k v | Some h -> add_unless_exists h k v
let get_multi h k =
  let k = LString.of_string k in
  try StringMap.find k h with | Not_found  -> []
let map fn h =
  StringMap.mapi (fun k  -> fun v  -> fn (LString.to_string k) v) h
let iter fn h = ignore (map fn h)
let fold fn h acc =
  StringMap.fold
    (fun k  ->
       fun v  ->
         fun acc  ->
           List.fold_left
             (fun acc  -> fun v  -> fn (LString.to_string k) v acc) acc v) h
    acc
let of_list l =
  List.fold_left (fun h  -> fun (k,v)  -> add h k v) (init ()) l
let to_list h =
  List.rev (fold (fun k  -> fun v  -> fun acc  -> (k, v) :: acc) h [])
let header_line k v = Printf.sprintf "%s: %s\r\n" k v
let to_lines h =
  List.rev
    (fold (fun k  -> fun v  -> fun acc  -> (header_line k v) :: acc) h [])
let to_frames =
  let to_frame k v acc = (Printf.sprintf "%s: %s" k v) :: acc in
  fun h  -> List.rev (fold to_frame h [])
let to_string h =
  let b = Buffer.create 128 in
  h |>
    (iter
       (fun k  ->
          fun v  ->
            v |>
              (List.iter
                 (fun v  ->
                    Buffer.add_string b k;
                    Buffer.add_string b ": ";
                    Buffer.add_string b v;
                    Buffer.add_string b "\r\n"))));
  Buffer.add_string b "\r\n";
  Buffer.contents b
let parse_content_range s =
  try
    let (start,fini,total) =
      Scanf.sscanf s "bytes %Ld-%Ld/%Ld"
        (fun start  -> fun fini  -> fun total  -> (start, fini, total)) in
    Some (start, fini, total)
  with | Scanf.Scan_failure _ -> None
let get_content_range headers =
  match get headers "content-length" with
  | Some clen -> (try Some (Int64.of_string clen) with | _ -> None)
  | None  ->
      (match get headers "content-range" with
       | Some range_s ->
           (match parse_content_range range_s with
            | Some (start,fini,total) ->
                if
                  (fini < total) &&
                    ((start <= total) && ((0L <= start) && (0L <= total)))
                then
                  let num_bytes_to_read = Int64.add (Int64.sub fini start) 1L in
                  Some num_bytes_to_read
                else None
            | None  -> None)
       | None  -> None)
let get_connection_close headers =
  match get headers "connection" with | Some "close" -> true | _ -> false
let media_type_re s =
  match Js.Re.exec s ([%bs.re "/[ \t]*([^ \t];)+/i"]) with
  | Some m -> m
  | None  -> raise Not_found
let get_first_match _re s =
  try
    let rs = media_type_re s in
    let subs = Js.Re.captures rs in Js.toOption (subs.(1))
  with | Not_found  -> None
let get_media_type headers =
  match get headers "content-type" with
  | Some s -> get_first_match media_type_re s
  | None  -> None
let get_acceptable_media_ranges headers =
  Accept.media_ranges (get headers "accept")
let get_acceptable_charsets headers =
  Accept.charsets (get headers "accept-charset")
let get_acceptable_encodings headers =
  Accept.encodings (get headers "accept-encoding")
let get_acceptable_languages headers =
  Accept.languages (get headers "accept-language")
let get_transfer_encoding headers =
  match get headers "transfer-encoding" with
  | Some "chunked" -> Transfer.Chunked
  | Some _|None  ->
      (match get_content_range headers with
       | Some len -> Transfer.Fixed len
       | None  -> Transfer.Unknown)
let add_transfer_encoding headers enc =
  let open Transfer in
    match ((get_transfer_encoding headers), enc) with
    | (Fixed _,_)|(Chunked ,_) -> headers
    | (Unknown ,Chunked ) -> add headers "transfer-encoding" "chunked"
    | (Unknown ,Fixed len) ->
        add headers "content-length" (Int64.to_string len)
    | (Unknown ,Unknown ) -> headers
let is_form headers =
  (get_media_type headers) = (Some "application/x-www-form-urlencoded")
let get_location headers =
  match get headers "location" with
  | None  -> None
  | Some u -> Some (Uri.of_string u)
let prepend_user_agent headers user_agent =
  let k = "user-agent" in
  match get headers k with
  | Some ua -> replace headers k (user_agent ^ (" " ^ ua))
  | None  -> add headers k user_agent
let connection h =
  match get h "connection" with
  | Some v when v = "keep-alive" -> Some `Keep_alive
  | Some v when v = "close" -> Some `Close
  | Some x -> Some (`Unknown x)
  | _ -> None
