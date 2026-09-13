// trolley.js - Trolley model & path helpers

class Trolley {
  constructor(id, pathCurve) {
    this.id = id;
    this.path = pathCurve; // THREE.CatmullRomCurve3
    this.progress = 0; // 0..1
    this.speed = 0.02; // base
    this.mesh = null;
    this.state = 'idle'; // idle,moving,loading,charging,fault
    this.battery = 1.0;
    this.lastUpdate = performance.now();
  }

  attachMesh(mesh) { this.mesh = mesh; }

  update(dt) {
    if (this.state === 'moving') {
      this.progress += this.speed * dt;
      if (this.progress > 1) this.progress = 0; // loop for demo
      const pos = this.path.getPointAt(this.progress);
      const tangent = this.path.getTangentAt(this.progress);
      this.mesh.position.copy(pos);
      // orientation smoothing
      const up = new THREE.Vector3(0,1,0);
      const targetQ = new THREE.Quaternion().setFromUnitVectors(new THREE.Vector3(0,0,1), tangent.clone().normalize());
      this.mesh.quaternion.slerp(targetQ, Math.min(0.2, dt*3));
      // slight tilt for realism based on turning (curvature)
      const tilt = tangent.x * 0.1;
      this.mesh.rotation.z = -tilt;
      this.battery -= 0.0002 * dt;
      if (this.battery < 0.15) this.state = 'charging';
    } else if (this.state === 'charging') {
      this.battery += 0.0005 * dt;
      if (this.battery > 0.98) this.state = 'idle';
    }
    // update color/material based on state outside
  }
}

window.Trolley = Trolley;
