// redis_fallback.js - Nodejs example combining in-memory LRU fallback with Redis
const Redis = require('ioredis');
const redis = new Redis({ host: '127.0.0.1', port: 6379 });
const LRU = require('lru-cache');
const lru = new LRU({ max: 5000 });

async function getItem(key, loader) {
  const cached = lru.get(key);
  if (cached) return cached;
  const v = await redis.get(key);
  if (v) { const obj = JSON.parse(v); lru.set(key, obj); return obj; }
  // fallback loader (DB)
  const fresh = await loader();
  if (fresh) {
    redis.set(key, JSON.stringify(fresh), 'EX', 60);
    lru.set(key, fresh);
  }
  return fresh;
}
