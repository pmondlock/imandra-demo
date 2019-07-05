let make = () => {
  let connection =
    KnexConfig.Connection.make(
      ~user="postgres",
      ~password="",
      ~host="localhost",
      ~database="demo",
      (),
    );

  KnexConfig.make(
    ~client="pg",
    ~connection,
    ~acquireConnectionTimeout=2000,
    (),
  )
  |> Knex.make;
};

let rows = result =>
  (
    switch (result##rows |> Js.Null_undefined.toOption) {
    | Some(x) => Belt.Result.Ok(x)
    | None => Belt.Result.Error(`RowsNotFound)
    }
  )
  |> Future.value;

let convPgError = Obj.magic;

let run = (errMsgFn, promise) =>
  FutureJs.fromPromise(promise, err =>
    try (
      /* This is super dangerous but there's no real nice way to extract information from the postgres errors from here */
      switch (
        convPgError(err)->Js.Dict.get("code"),
        convPgError(err)->Js.Dict.get("detail"),
      ) {
      | (Some("23505"), Some(detail)) => `DBConflict(detail)
      | _ => `DBError(errMsgFn(err))
      }
    ) {
    | _ => `DBError(errMsgFn(err))
    }
  );
