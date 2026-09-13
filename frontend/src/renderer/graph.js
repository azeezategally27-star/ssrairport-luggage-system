// simple cytoscape wrapper
window.renderGraph = function(graph) {
  const elements = [];
  graph.nodes.forEach(n => elements.push({ data: { id: n.id, label: n.label } }));
  graph.edges.forEach(e => elements.push({ data: { source: e.from, target: e.to } }));

  if (window.cy) {
    window.cy.elements().remove();
    window.cy.add(elements);
    window.cy.layout({ name: 'dagre' }).run();
    return;
  }

  window.cy = cytoscape({
    container: document.getElementById('cy'),
    elements: elements,
    style: [
      { selector: 'node', style: { 'content': 'data(label)', 'text-valign': 'center', 'color':'#222', 'background-color': '#9fd' } },
      { selector: 'edge', style: { 'width': 3, 'line-color': '#ccc', 'target-arrow-color': '#ccc', 'target-arrow-shape': 'triangle' } }
    ],
    layout: { name: 'cose' }
  });

  window.cy.on('tap', 'node', function(evt){
    const node = evt.target;
    document.getElementById('agentDetail').innerText = node.data('label');
  });
};
