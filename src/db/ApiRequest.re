open Util.Infix;
module Result = Belt.Result;

let save = (db: Knex.t, date, email) =>
  Knex.(
    (
      db
      |> rawBindingsArray(
           {| INSERT INTO requests (date, email) VALUES (?, ?) RETURNING id|},
           [|date, email|],
         )
      |> Db.run(err => Js.Console.error2("Database error", err))
    )
    +>>= Db.rows
    @<$> (
      fun
      | Ok([|row|]) => Result.Ok(row##id)
      | Ok(ls) => Result.Error(`InvalidRowCount((1, ls |> Array.length)))
      | Error(err) => Result.Error(`DbError(err))
    )
  );
