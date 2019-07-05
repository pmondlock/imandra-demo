#!/usr/bin/env bash

DBMATE=$(which dbmate)

if [[ ! -f "${DBMATE}" ]]; then
  echo "please install dbmate and verify it is in your path"
  exit 2
fi

if [[ ! -x "${DBMATE}" ]]; then
  echo "dbmate found but not executable, please fix and rerun"
  exit 1
fi

"${DBMATE}" up
