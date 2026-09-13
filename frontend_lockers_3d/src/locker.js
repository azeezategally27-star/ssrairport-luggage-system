// locker.js - Locker object with animations and state

class Locker {
  constructor(id, position, size, scene, animSpeed=1) {
    this.id = id;
    this.position = position.clone();
    this.size = size; // {w,h,d}
    this.scene = scene;
    this.state = 'vacant'; // vacant, booked, occupied, priority, maintenance
    this.animSpeed = animSpeed;
    this.openProgress = 0; // 0..1, 1=open
    this.slideOffset = 0;
    this.createMesh();
    this.pulse = 0;
  }

  createMesh() {
    const mat = new THREE.MeshStandardMaterial({ color: 0xffffff, metalness: 0.8, roughness: 0.2 });
    const geo = new THREE.BoxGeometry(this.size.w, this.size.h, this.size.d);
    this.mesh = new THREE.Mesh(geo, mat);
    this.mesh.position.copy(this.position);
    this.scene.add(this.mesh);
    // small ID label
    this.label = createLabel(this.id);
    this.label.position.set(this.position.x, this.position.y + this.size.h/2 + 0.6, this.position.z);
    this.scene.add(this.label);
  }

  setState(state) {
    this.state = state;
    this.lastStateChange = performance.now();
  }

  toggleOpen() { this.openTarget = (this.openTarget && this.openTarget>0)?0:1; }

  update(dt) {
    // color logic
    let targetColor = new THREE.Color(0xffffff);
    if (this.state === 'vacant') targetColor.setHex(0xffffff);
    else if (this.state === 'booked') targetColor.setHex(0x00d4e0); // cyan
    else if (this.state === 'priority') targetColor.setHex(0xff8a3d); // orange
    else if (this.state === 'occupied') targetColor.setHex(0xa6f3c7); // green
    else if (this.state === 'maintenance') targetColor.setHex(0xcccccc);

    // pulse for priority
    if (this.state === 'priority') {
      this.pulse += dt * 2 * this.animSpeed;
      const pulseMod = 0.15 * Math.sin(this.pulse);
      targetColor.offsetHSL(0, 0, pulseMod);
    }

    // lerp material color
    this.mesh.material.color.lerp(targetColor, Math.min(1, dt * 3 * this.animSpeed));

    // open/slide animation (simulate drawer sliding forward)
    if (this.openTarget === undefined) this.openTarget = 0;
    const diff = this.openTarget - this.openProgress;
    this.openProgress += diff * Math.min(1, dt * 4 * this.animSpeed);
    // slide along local Z by 0..0.6
    this.mesh.position.z = this.position.z - this.openProgress * 0.6;

    // subtle rotation for metallic feel
    this.mesh.rotation.y += 0.001 * dt * this.animSpeed;
    // update label color for contrast
    updateLabel(this.label, this.state);
  }
}

function createLabel(text) {
  const canvas = document.createElement('canvas'); canvas.width = 256; canvas.height = 64;
  const ctx = canvas.getContext('2d'); ctx.fillStyle = '#1a1a1a'; ctx.font = '24px sans-serif'; ctx.fillText(text, 4, 28);
  const texture = new THREE.CanvasTexture(canvas);
  const sprite = new THREE.Sprite(new THREE.SpriteMaterial({ map: texture })); sprite.scale.set(2.8,0.7,1);
  return sprite;
}

function updateLabel(sprite, state) {
  // tint label background by state if needed (omitted for brevity). Keep readable.
}

window.Locker = Locker;
