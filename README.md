# Imandra demo app

## Setup

1. Clone this repo
2. Install [dbmate](https://github.com/amacneil/dbmate)
3. Install [fnm](https://github.com/Schniz/fnm)
4. Install [yarn](https://yarnpkg.com/en/)
5. Install [postgresql](https://github.com/bitnami/bitnami-docker-postgresql) with no master password mode enabled

## Building and running

```bash
DATABASE_URL="postgres://postgres@localhost/demo?sslmode=disable" yarn db:up
yarn start
```

## Rebuilding codecs

1. Install opam and ocaml switch 4.06+, and `eval $(opam env)`
2. Run `opam pin add atd --dev-repo` and `opam pin add atdgen --dev-repo`
3. Run `yarn atdgen`
