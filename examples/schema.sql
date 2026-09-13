-- schema.sql - partitioned events table (Postgres)

CREATE TABLE events (
  id BIGSERIAL PRIMARY KEY,
  flight_id TEXT NOT NULL,
  event_type TEXT NOT NULL,
  payload JSONB,
  ts TIMESTAMPTZ NOT NULL DEFAULT now()
) PARTITION BY RANGE (ts);

CREATE TABLE events_2026_09 PARTITION OF events FOR VALUES FROM ('2026-09-01') TO ('2026-10-01');

CREATE INDEX ON events (flight_id, ts DESC);
