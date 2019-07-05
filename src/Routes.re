open Util.Auth;
open Util.Infix;
open Util;
open Express;

let testRoute =
  get("/makes", (req, res) =>
    (req |> requireRole("admin") |> Future.value)
    +>>= (
      (token: Jwt_t.accessToken) =>
        ApiRequest.save(Db.make(), "2019-01-01", token.sub)
    )
    +|- (id => Js.Console.log("Wrote id " ++ id))
    +>>= (id => Service.getAllMakes())
    +<$> (
      makeResponse =>
        Api_t.{
          results:
            makeResponse.results
            |> List.fold_left(
                 (a, m: Vpic_t.make_entry) => [m.name, ...a],
                 [],
               ),
        }
    )
    +<$> CodecUtil.encode(Api_bs.write_make_response)
    @<$> (
      fun
      | Ok(json) => res |> Response.sendJson(json)
      | Error(`Unauthorized) => res |> Response.sendStatus(Unauthorized)
      | Error(err) => {
          Js.Console.log2("error!", err);
          res
          |> Response.status(InternalServerError)
          |> Response.sendString("error");
        }
    )
  );
