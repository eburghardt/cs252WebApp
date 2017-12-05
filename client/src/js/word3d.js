if(!Detector.webgl) {
	Detector.addGetWebGLMessage();
	document.getElementByID('container').innerHTML = "";
}
var container;
var camera, controls, scene, renderer;
var cube;
var mesh;
var raycaster;
var controlsEnabled;
	
var rollOverMesh, rollOverMaterial;
var cubeGeo, cubeMaterial;
	
var clock = new THREE.Clock();

var objects = [];

var boardWidth = 15, boardDepth = 15, boardHeight = 15;
		
var blocker = document.getElementById('blocker');
var instructions = document.getElementById('instructions');
		
var havePointerLock = 'pointerLockElement' in document || 'mozPointerLockElement' in document || 'webkitPointerLockElement' in document;

if(havePointerLock) {
	var element = document.body;

	var pointerlockchange = function(event) {
		if(document.pointerLockElement === element || document.mozPointerLockElement === element || document.webkitPointerLockElement === element) {
			controlsEnabled = true;
			controls.enabled = true;
	
			blocker.style.display = 'none';
		} else {
			controlsEnabled = false;
			controls.enabled = false;

			blocker.style.display = 'block'
			instructions.style.display = '';
		}
	};

	var pointerlockerror = function(event) {
		instructions.style.display = '';
	};

	//Hook pointer lock state change events
	document.addEventListener('pointerlockchange', pointerlockchange, false);
	document.addEventListener('mozpointerlockchange', pointerlockchange, false);
	document.addEventListener('webkitpointerlockchange', pointerlockchange, false);

	document.addEventListener('pointerlockerror', pointerlockerror, false);
	document.addEventListener('mozpointerlockerror', pointerlockerror, false);
	document.addEventListener('webkitpointerlockerror', pointerlockerror, false);
				
	instructions.addEventListener('click', function( event ) {

		instructions.style.display = 'none';

		//ask the browser to lock the pointer
		element.requestPointerLock = element.requestPointerLock || element.mozRequestPointerLock || element.webkitrequestpointerlock;
		element.requestPointerLock();
	}, false);
			
} else {
	instructions.innerHTML = 'Your browser doesn\'t seem to support Pointer Lock API';
}

init();
animate();

function init() {
	container = document.getElementById('container');
	camera = new THREE.PerspectiveCamera(75, window.innerWidth / window.innerHeight, 1, 3000);
	//camera.position.y = getY(7, 7) * 100 + 100;
				
	scene = new THREE.Scene();
	scene.background = new THREE.Color(0xf5f5f5);
				
	//Roll-over helper
	var rollOverGeo = new THREE.BoxGeometry(64, 64, 64);
	rollOverMaterial = new THREE.MeshBasicMaterial( {color: 0x00ff00, opacity: 0.5, transparent: true});
	rollOverMesh = new THREE.Mesh(rollOverGeo, rollOverMaterial);
	scene.add(rollOverMesh);

	cubePosMesh = new THREE.Mesh(rollOverGeo, rollOverMaterial);
	cubePosMesh.position.y = cubePosMesh.position.y + 64;
	scene.add(cubePosMesh);	

	//cubes
	var texture = new THREE.TextureLoader().load('../../assets/a.png');
	texture.magFilter = THREE.LinearMipMapLinearFilter;
	texture.minFilter = THREE.LinearMipMapLinearFilter;

	cubeGeo = new THREE.BoxGeometry(64, 64, 64);				
	cubeMaterial = new THREE.MeshLambertMaterial({map: texture});
	
	//grids
	var gridHelper = new THREE.GridHelper(960, 15);
	scene.add(gridHelper);

	//raycasting
	//raycaster = new THREE.Raycaster();
	//mouse = new THREE.Vector2();
	
	var geometry = new THREE.PlaneBufferGeometry(960, 960);
	geometry.rotateX( - Math.PI / 2);

	plane = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial( {visible: false }));
	scene.add(plane);
	
	objects.push(plane);
	
	controls = new THREE.PointerLockControls(camera);
	
	controls.movementSpeed = 1000;
	controls.lookSpeed = 0.002;
	controls.enabled = true;
	scene.add(controls.getObject());
	
	
			
	controls.getObject().position.x = objects[0].position.x + 100;
	controls.getObject().position.y = objects[0].position.y + 100;
	controls.getObject().position.z = objects[0].position.z + 100;
					

	/*
	controls.lookVertical = true;
	*/

	//Lights
	var ambientLight = new THREE.AmbientLight(0xffffff);
	scene.add(ambientLight);

	renderer = new THREE.WebGLRenderer({antialias: true});
	renderer.setPixelRatio(window.devicePixelRatio);
	renderer.setSize(window.innerWidth, window.innerHeight);
	container.appendChild(renderer.domElement);
	
	document.addEventListener('click', onDocumentClick, false);
	document.addEventListener('mousemove', onDocumentMouseMove, false);				
	
	window.addEventListener('resize', onWindowResize, false);
}

function getY(x, z) {
	return 10;
}

function onWindowResize() {
	camera.aspect = window.innerWidth / window.innerHeight;
	camera.updateProjectionMatrix();
	
	renderer.setSize(window.innerWidth, window.innerHeight);
}

function onDocumentMouseMove(event) {
				
	raycaster = new THREE.Raycaster();
	var getDirection = controls.getDirection();
	var direction = new THREE.Vector3(0, 0, -1);
	getDirection(direction);

	
	raycaster.ray.direction.copy(direction);
	raycaster.ray.origin.copy(controls.getObject().position);
	console.log(raycaster.ray.direction);
	/*
	vector.sub(controls.getObject().position); //Vector is now a unit vector with same direction as camera
	raycaster = new THREE.Raycaster(controls.getObject().position, vector);
	*/
	var intersects = raycaster.intersectObjects(objects);
	
	if(intersects.length > 0) {
		rollOverMesh.visible = true;
		var intersect = intersects[0];
		rollOverMesh.position.copy(intersect.point).add(intersect.face.normal);
		rollOverMesh.position.divideScalar(64).floor().multiplyScalar(64).addScalar(32);
	} else {
		rollOverMesh.visible = false;
	}
};
			
	
function onDocumentClick(event) {
	//event.preventDefault();
	console.log(controlsEnabled);
	if(controlsEnabled) {	
		var intersects = raycaster.intersectObjects(objects);
		if(intersects.length > 0) {
			var intersect = intersects[0];
			var voxel = new THREE.Mesh(cubeGeo, cubeMaterial);
			voxel.position.copy(intersect.point).add(intersect.face.normal);
			voxel.position.divideScalar(64).floor().multiplyScalar(64).addScalar(32);
			scene.add(voxel);
			objects.push(voxel);
		}
	}				
}

function animate() {
	requestAnimationFrame(animate);
	
	render();
}

function render() {
	controls.update(clock.getDelta());
	renderer.render(scene, camera);
}