open Util.Infix;
module Result = Belt.Result;

let getAllMakes = () =>
  Client.get(
    ~headers=Header.init(),
    "https://vpic.nhtsa.dot.gov/api/vehicles/getallmakes?format=json"
    |> Uri.of_string,
  )
  +>>= Client.json_of_res
  +>>= (CodecUtil.decode(Vpic_bs.read_make_response) >> Future.value);
