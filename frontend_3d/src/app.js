// app.js: Three.js scene and UI bindings
let scene, camera, renderer, controls;
let chain = new DemoChain();
let blockMeshes = [];

function initThree() {
  const canvas = document.getElementById('scene');
  renderer = new THREE.WebGLRenderer({ canvas, antialias: true, alpha: true });
  renderer.setSize(canvas.clientWidth || canvas.width, canvas.clientHeight || canvas.height);
  renderer.setPixelRatio(window.devicePixelRatio || 1);

  scene = new THREE.Scene();
  scene.background = new THREE.Color(0xfff7fb);

  camera = new THREE.PerspectiveCamera(50, canvas.clientWidth / canvas.clientHeight, 0.1, 1000);
  camera.position.set(0, 6, 12);

  controls = new THREE.OrbitControls(camera, renderer.domElement);
  controls.enableDamping = true;

  const light = new THREE.DirectionalLight(0xffffff, 0.9);
  light.position.set(5,10,7);
  scene.add(light);
  scene.add(new THREE.AmbientLight(0xffffff, 0.6));

  window.addEventListener('resize', onWindowResize);

  animate();
}

function onWindowResize() {
  const canvas = document.getElementById('scene');
  camera.aspect = canvas.clientWidth / canvas.clientHeight;
  camera.updateProjectionMatrix();
  renderer.setSize(canvas.clientWidth, canvas.clientHeight);
}

async function addBlockToScene(block) {
  const geometry = new THREE.BoxGeometry(1.6,1.2,1.2);
  const mat = new THREE.MeshStandardMaterial({ color: 0xffc0dd, metalness:0.3, roughness:0.6 });
  const mesh = new THREE.Mesh(geometry, mat);
  const idx = block.index;
  mesh.position.set(idx*2.2 - (chain.chain.length-1), 0, 0);
  mesh.userData = { index: idx };
  scene.add(mesh);
  blockMeshes.push(mesh);

  // add connector
  if (idx>0) {
    const from = blockMeshes[idx-1].position;
    const to = mesh.position;
    const dir = new THREE.Vector3().subVectors(to, from);
    const len = dir.length();
    const cylGeo = new THREE.CylinderGeometry(0.06,0.06,len,8);
    const cylMat = new THREE.MeshStandardMaterial({ color:0xffb6d5, emissive:0xffe6f2, emissiveIntensity:0.4 });
    const cyl = new THREE.Mesh(cylGeo, cylMat);
    const mid = new THREE.Vector3().addVectors(from, to).multiplyScalar(0.5);
    cyl.position.copy(mid);
    cyl.quaternion.setFromUnitVectors(new THREE.Vector3(0,1,0), dir.clone().normalize());
    scene.add(cyl);
  }
}

function animate() {
  requestAnimationFrame(animate);
  controls.update();
  renderer.render(scene, camera);
}

async function uiAddBlock(passenger, luggage, event) {
  const blk = await chain.addBlock(passenger, luggage, event);
  await addBlockToScene(blk);
  document.getElementById('blockDetail').innerText = JSON.stringify(blk, null, 2);
}

async function uiValidate() {
  const res = await chain.validate();
  res.forEach(r => {
    const mesh = blockMeshes[r.index];
    if (!mesh) return;
    mesh.material.color.set(r.ok ? 0xb7ffcf : 0xffb6b6);
  });
}

function uiExport() {
  const data = JSON.stringify(chain.toJSON(), null, 2);
  const blob = new Blob([data], { type: 'application/json' });
  const url = URL.createObjectURL(blob);
  const a = document.createElement('a');
  a.href = url; a.download = 'ledger.json'; a.click();
}

async function uiTamper() {
  if (!chain.chain.length) return alert('No blocks to tamper');
  // tamper middle block if possible
  const idx = Math.floor(chain.chain.length / 2);
  chain.chain[idx].event = chain.chain[idx].event + ' (tampered)';
  // do NOT recompute hash to simulate tamper
  alert('Tampered block ' + idx);
}

function setupUI() {
  document.getElementById('btnAdd').addEventListener('click', async () => {
    const p = document.getElementById('passengerId').value;
    const l = document.getElementById('luggageId').value;
    const e = document.getElementById('eventType').value;
    await uiAddBlock(p, l, e);
  });
  document.getElementById('btnRandom').addEventListener('click', async () => {
    const p = 'P' + Math.floor(Math.random()*900+100);
    const l = 'L' + Math.floor(Math.random()*900+100);
    const events = ['Check-in','Security Scan','Loaded','Cleared'];
    const e = events[Math.floor(Math.random()*events.length)];
    await uiAddBlock(p,l,e);
  });
  document.getElementById('btnValidate').addEventListener('click', uiValidate);
  document.getElementById('btnExport').addEventListener('click', uiExport);
  document.getElementById('btnTamper').addEventListener('click', uiTamper);

  // click detection
  const raycaster = new THREE.Raycaster();
  const mouse = new THREE.Vector2();
  renderer.domElement.addEventListener('click', (ev) => {
    const rect = renderer.domElement.getBoundingClientRect();
    mouse.x = ((ev.clientX - rect.left) / rect.width) * 2 - 1;
    mouse.y = -((ev.clientY - rect.top) / rect.height) * 2 + 1;
    raycaster.setFromCamera(mouse, camera);
    const intersects = raycaster.intersectObjects(blockMeshes);
    if (intersects.length) {
      const m = intersects[0].object;
      const idx = m.userData.index;
      const blk = chain.chain[idx];
      document.getElementById('blockDetail').innerText = JSON.stringify(blk, null, 2);
    }
  });
}

window.addEventListener('DOMContentLoaded', () => { initThree(); setupUI(); });
