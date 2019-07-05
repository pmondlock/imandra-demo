-- migrate:up
create extension pgcrypto;

create table requests (
  id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
  date TIMESTAMP NOT NULL,
  email VARCHAR(1024) NOT NULL
);

-- migrate:down
drop table requests;
drop extension pgcrypto;
