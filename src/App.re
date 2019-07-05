open Express;

[%raw "require('isomorphic-fetch')"];

let app = express();

[%raw {| app.use(require('body-parser').json({type: '*/json'})) |}];

let app = Routes.testRoute(app);

let port = 3001;

let onListen = e =>
  switch (e) {
  | exception (Js.Exn.Error(e)) =>
    Js.Console.error(e);
    Node.Process.exit(1);
  | _ => print_endline @@ {j|Listening at http://127.0.0.1:$port|j}
  };

let server = App.listen(app, ~port, ~onListen, ());
