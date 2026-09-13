// ledger.js: simple blockchain model for demo
class Block {
  constructor(index, timestamp, passenger_id, luggage_id, event, prev_hash) {
    this.index = index;
    this.timestamp = timestamp;
    this.passenger_id = passenger_id;
    this.luggage_id = luggage_id;
    this.event = event;
    this.prev_hash = prev_hash || '0';
    this.hash = null; // computed later
  }

  async computeHash() {
    const msg = `${this.index}|${this.timestamp}|${this.passenger_id}|${this.luggage_id}|${this.event}|${this.prev_hash}`;
    const enc = new TextEncoder().encode(msg);
    const digest = await crypto.subtle.digest('SHA-256', enc);
    const hashArray = Array.from(new Uint8Array(digest));
    this.hash = hashArray.map(b => b.toString(16).padStart(2,'0')).join('');
    return this.hash;
  }
}

class Chain {
  constructor() { this.chain = []; }
  async addBlock(passenger_id, luggage_id, event) {
    const idx = this.chain.length;
    const prev = idx ? this.chain[idx-1].hash : '0';
    const blk = new Block(idx, new Date().toISOString(), passenger_id, luggage_id, event, prev);
    await blk.computeHash();
    this.chain.push(blk);
    return blk;
  }
  async validate() {
    const results = [];
    for (let i=0;i<this.chain.length;i++) {
      const blk = this.chain[i];
      const expected = await (async () => {
        const msg = `${blk.index}|${blk.timestamp}|${blk.passenger_id}|${blk.luggage_id}|${blk.event}|${blk.prev_hash}`;
        const enc = new TextEncoder().encode(msg);
        const digest = await crypto.subtle.digest('SHA-256', enc);
        const hashArray = Array.from(new Uint8Array(digest));
        return hashArray.map(b => b.toString(16).padStart(2,'0')).join('');
      })();
      const ok = (expected === blk.hash) && (i===0 || blk.prev_hash === this.chain[i-1].hash);
      results.push({ index: i, ok });
    }
    return results;
  }
  toJSON() { return this.chain.map(b=>({ index:b.index, timestamp:b.timestamp, passenger_id:b.passenger_id, luggage_id:b.luggage_id, event:b.event, prev_hash:b.prev_hash, hash:b.hash })); }
}

window.DemoChain = Chain;
