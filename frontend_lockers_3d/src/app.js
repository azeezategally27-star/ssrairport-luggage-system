// app.js - main scene, spawning grid and UI bindings

let scene, camera, renderer, controls, clock;
let lockers = [];
let animSpeed = 1;

function init() {
  const canvas = document.getElementById('scene');
  renderer = new THREE.WebGLRenderer({ canvas, antialias:true });
  renderer.setSize(canvas.clientWidth || canvas.width, canvas.clientHeight || canvas.height);
  renderer.setPixelRatio(window.devicePixelRatio || 1);
  scene = new THREE.Scene(); scene.background = new THREE.Color(0xfffbff);
  camera = new THREE.PerspectiveCamera(50, canvas.clientWidth/canvas.clientHeight, 0.1, 1000);
  camera.position.set(0, 20, 40);
  controls = new THREE.OrbitControls(camera, renderer.domElement);
  controls.target.set(0,0,0);

  scene.add(new THREE.AmbientLight(0xffffff, 0.8));
  const dir = new THREE.DirectionalLight(0xffffff, 0.6); dir.position.set(5,10,7); scene.add(dir);

  // floor grid
  const grid = new THREE.GridHelper(80, 40, 0xffe8f1, 0xffe8f1); scene.add(grid);

  clock = new THREE.Clock();
  createGrid(3,8,2);
  animate();
}

function createGrid(rows, cols, levels) {
  // clear old
  for (let l of lockers) { scene.remove(l.mesh); scene.remove(l.label); }
  lockers = [];
  const spacing = 2.4; const w=2.0,h=1.6,d=1.2;
  const startX = - (cols-1) * spacing / 2;
  for (let level=0; level<levels; level++) {
    for (let r=0; r<rows; r++) {
      for (let c=0; c<cols; c++) {
        const id = `L${level+1}-R${r+1}-C${c+1}`;
        const pos = new THREE.Vector3(startX + c*spacing, level*(h+0.6) + h/2 + 0.2, - r*(spacing));
        const locker = new Locker(id, pos, {w:h,d:w,h:1.6}, scene, animSpeed);
        lockers.push(locker);
      }
    }
  }
  updateMetrics();
}

function updateMetrics() {
  document.getElementById('metrics').innerText = 'Lockers: ' + lockers.length + '  Vacant: ' + lockers.filter(l=>l.state==='vacant').length;
}

function animate() {
  requestAnimationFrame(animate);
  const dt = clock.getDelta() * 60; // normalized
  for (let l of lockers) l.update(dt * animSpeed);
  renderer.render(scene, camera);
}

window.addEventListener('DOMContentLoaded', () => {
  init();
  document.getElementById('btnApply').addEventListener('click', ()=>{
    const rows = parseInt(document.getElementById('rows').value,10);
    const cols = parseInt(document.getElementById('cols').value,10);
    const levels = parseInt(document.getElementById('levels').value,10);
    createGrid(rows, cols, levels);
  });
  document.getElementById('btnOpen').addEventListener('click', ()=>{
    const sel = document.getElementById('sel').innerText; if (!sel || sel==='None') return; const locker = lockers.find(l=>l.id===sel); if (locker) { locker.openTarget = 1; logEvent('open', locker.id); }
  });
  document.getElementById('btnSetPriority').addEventListener('click', ()=>{
    const sel = document.getElementById('sel').innerText; if (!sel || sel==='None') return; const locker = lockers.find(l=>l.id===sel); if (locker) { locker.setState(locker.state==='priority'?'vacant':'priority'); logEvent('priority', locker.id); }
  });
  document.getElementById('btnMaintenance').addEventListener('click', ()=>{
    const sel = document.getElementById('sel').innerText; if (!sel || sel==='None') return; const locker = lockers.find(l=>l.id===sel); if (locker) { locker.setState(locker.state==='maintenance'?'vacant':'maintenance'); logEvent('maintenance', locker.id); updateMetrics(); }
  });
  document.getElementById('btnAutoBook').addEventListener('click', ()=> simulateBookings());
  document.getElementById('btnClear').addEventListener('click', ()=> { for (let l of lockers) l.setState('vacant'); logEvent('clear', 'all'); updateMetrics(); });
  document.getElementById('btnExport').addEventListener('click', ()=> exportConfig());
  document.getElementById('animSpeed').addEventListener('input', (e)=>{ animSpeed = parseFloat(e.target.value); });

  // pick on click
  renderer.domElement.addEventListener('pointerdown', onPointerDown);
});

function onPointerDown(e) {
  const rect = renderer.domElement.getBoundingClientRect();
  const x = ((e.clientX - rect.left) / rect.width) * 2 -1;
  const y = -((e.clientY - rect.top) / rect.height) * 2 +1;
  const mouse = new THREE.Vector2(x,y);
  const ray = new THREE.Raycaster(); ray.setFromCamera(mouse, camera);
  const meshes = lockers.map(l=>l.mesh);
  const hits = ray.intersectObjects(meshes);
  if (hits.length) {
    const m = hits[0].object; const locker = lockers.find(l=>l.mesh===m);
    if (locker) { document.getElementById('sel').innerText = locker.id; logEvent('select', locker.id); }
  }
}

function logEvent(type, id) {
  const el = document.getElementById('events'); const now = new Date().toISOString(); const s = `${now} | ${type} | ${id}`; const p = document.createElement('div'); p.innerText = s; el.prepend(p);
}

function simulateBookings() {
  // pick random vacant lockers and book them
  const vacant = lockers.filter(l=>l.state==='vacant');
  for (let i=0;i<Math.min(6, vacant.length); i++) { const l = vacant[Math.floor(Math.random()*vacant.length)]; l.setState('booked'); logEvent('book', l.id); }
  updateMetrics();
}

function exportConfig() {
  const data = { lockers: lockers.map(l=>({id:l.id,state:l.state})) };
  const txt = JSON.stringify(data, null, 2);
  const blob = new Blob([txt], {type: 'application/json'});
  const url = URL.createObjectURL(blob); const a = document.createElement('a'); a.href=url; a.download='lockers_export.json'; a.click(); URL.revokeObjectURL(url);
}
