#!/usr/bin/env bash
#

atdgen -t src/codec/jwt.atd && atdgen -bs src/codec/jwt.atd -open Jwt_t
atdgen -t src/codec/vpic.atd && atdgen -bs src/codec/vpic.atd -open Vpic_t
atdgen -t src/codec/api.atd && atdgen -bs src/codec/api.atd -open Api_t
