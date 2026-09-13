// app.js - Three.js scene and simulation control

let scene, camera, renderer, controls, clock;
let trolleys = [];
let pathCurves = [];
let running = false;
let speedMultiplier = 1;
let selected = null;

function initScene() {
  const canvas = document.getElementById('scene');
  renderer = new THREE.WebGLRenderer({ canvas, antialias:true });
  renderer.setSize(canvas.clientWidth || canvas.width, canvas.clientHeight || canvas.height);
  renderer.setPixelRatio(window.devicePixelRatio || 1);
  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xfffbfd);

  camera = new THREE.PerspectiveCamera(45, canvas.clientWidth/canvas.clientHeight, 0.1, 1000);
  camera.position.set(0, 30, 60);
  controls = new THREE.OrbitControls(camera, renderer.domElement);
  controls.target.set(0,0,0);

  const light = new THREE.DirectionalLight(0xffffff, 0.9); light.position.set(10,20,10); scene.add(light);
  scene.add(new THREE.AmbientLight(0xffffff, 0.6));

  // ground
  const ground = new THREE.Mesh(new THREE.PlaneGeometry(200,200), new THREE.MeshStandardMaterial({color:0xfff7fb}));
  ground.rotation.x = -Math.PI/2; scene.add(ground);

  // create simple apron objects
  createApronScene();

  clock = new THREE.Clock();
  animate();
}

function createApronScene() {
  // create a few smooth paths using CatmullRom
  const p1 = [new THREE.Vector3(-20,0,-10), new THREE.Vector3(-10,0,0), new THREE.Vector3(0,0,10), new THREE.Vector3(20,0,10)];
  const p2 = [new THREE.Vector3(-20,0,10), new THREE.Vector3(-10,0,5), new THREE.Vector3(10,0,0), new THREE.Vector3(30,0,-10)];
  const c1 = new THREE.CatmullRomCurve3(p1); const c2 = new THREE.CatmullRomCurve3(p2);
  pathCurves.push(c1); pathCurves.push(c2);

  // visualise paths
  for (let c of pathCurves) {
    const pts = c.getPoints(100);
    const geom = new THREE.BufferGeometry().setFromPoints(pts);
    const mat = new THREE.LineBasicMaterial({ color: 0xffb6d5, linewidth: 3 });
    const line = new THREE.Line(geom, mat); scene.add(line);
  }

  // create charging dock
  const dock = new THREE.Mesh(new THREE.BoxGeometry(6,1,3), new THREE.MeshStandardMaterial({color:0xffeaf0}));
  dock.position.set(25,0.5,-15); scene.add(dock);
  const dockLabel = createTextSprite('Charging Dock'); dockLabel.position.set(25,3,-15); scene.add(dockLabel);

  // create stands
  for (let i=0;i<4;i++) {
    const stand = new THREE.Mesh(new THREE.BoxGeometry(8,1,6), new THREE.MeshStandardMaterial({color:0xfff1f5}));
    stand.position.set(-30 + i*18,0.5,18); scene.add(stand);
    const l = createTextSprite('Stand ' + (i+1)); l.position.set(-30 + i*18,3,18); scene.add(l);
  }
}

function createTextSprite(message) {
  const canvas = document.createElement('canvas'); const ctx = canvas.getContext('2d'); ctx.font = '24px Arial'; ctx.fillStyle = '#333'; ctx.fillText(message, 0,24);
  const tex = new THREE.CanvasTexture(canvas); const mat = new THREE.SpriteMaterial({ map: tex }); const sprite = new THREE.Sprite(mat); sprite.scale.set(8,2,1); return sprite;
}

function addTrolley(pathIdx) {
  const path = pathCurves[pathIdx % pathCurves.length];
  const t = new Trolley('T' + (trolleys.length+1), path);
  const geo = new THREE.BoxGeometry(2.2,1.0,1.2);
  const mat = new THREE.MeshStandardMaterial({ color: 0xff9fb8, metalness:0.2, roughness:0.6 });
  const mesh = new THREE.Mesh(geo, mat);
  scene.add(mesh);
  t.attachMesh(mesh);
  t.state = 'moving';
  t.speed = 0.02;
  t.progress = Math.random();
  trolleys.push(t);
  updateMetrics();
}

function updateMetrics() { document.getElementById('metrics').innerText = 'Trolleys: ' + trolleys.length; }

function animate() {
  requestAnimationFrame(animate);
  const dt = clock.getDelta() * speedMultiplier * 60; // scale delta for perceptual speed
  for (let t of trolleys) t.update(dt);
  renderer.render(scene, camera);
}

window.addEventListener('DOMContentLoaded', () => {
  initScene();
  document.getElementById('btnStart').addEventListener('click', ()=>{ running=true; if (trolleys.length===0) { addTrolley(0); addTrolley(1); } });
  document.getElementById('btnPause').addEventListener('click', ()=>{ running=false; });
  document.getElementById('btnAdd').addEventListener('click', ()=> addTrolley(Math.floor(Math.random()*pathCurves.length)));
  document.getElementById('btnBreak').addEventListener('click', ()=>{ if (trolleys.length) trolleys[0].state='fault'; });
  document.getElementById('speed').addEventListener('input', (e)=> { speedMultiplier = parseFloat(e.target.value); });
});
