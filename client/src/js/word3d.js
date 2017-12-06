if(!Detector.webgl) {
	Detector.addGetWebGLMessage();
	document.getElementByID('container').innerHTML = "";
}
var container;
var camera, controls, scene, renderer;
var raycaster;
var controlsEnabled;
	
var rollOverMesh, rollOverMaterial;
var cubeGeo, cubeMaterial;
	
var clock = new THREE.Clock();

var objects = [];
var textures = [];
var materials = [];
var testTexture;

var boardSize = 15;

var hand;
var handchars;
var handIndex = 0;

var handSelect;

var newPlay = [];
var newPlayHighlights = [];


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
	
	//hand
	hand = document.getElementById('handtable');
	handSelect = document.getElementById('handselecttable');	
		
	scene = new THREE.Scene();
	scene.background = new THREE.Color(0xf5f5f5);
				
	//Roll-over helper
	var rollOverGeo = new THREE.BoxGeometry(64, 64, 64);
	rollOverMaterial = new THREE.MeshBasicMaterial( {color: 0x00ff00, opacity: 0.5, transparent: true});
	rollOverMesh = new THREE.Mesh(rollOverGeo, rollOverMaterial);
	scene.add(rollOverMesh);

	var startCubeGeo = new THREE.BoxGeometry(30, 30, 30);
	cubePosMesh = new THREE.Mesh(startCubeGeo, rollOverMaterial);
	/*
	cubePosMesh.position.x = gridCoorToScreenCoor(7);
	cubePosMesh.position.y = gridCoorToScreenCoorY(7);
	cubePosMesh.position.z = gridCoorToScreenCoor(7);
	*/
	setGridPosition(cubePosMesh, 7, 7, 7);
	scene.add(cubePosMesh);	
	objects.push(cubePosMesh);

	//cubes
	initTextures();
	initMaterials();	

	hand.children[0].innerHTML = textures[0].image.outerHTML;
	hand.children[1].innerHTML = textures[1].image.outerHTML;
	hand.children[2].innerHTML = textures[2].image.outerHTML;
	hand.children[3].innerHTML = textures[3].image.outerHTML;
	hand.children[4].innerHTML = textures[4].image.outerHTML;
	hand.children[5].innerHTML = textures[5].image.outerHTML;
	hand.children[6].innerHTML = textures[6].image.outerHTML;
	handSelect.children[0].innerHTML = '<img crossorigin="Anonymous" src="../../assets/select.png">';
	for(i = 1; i < 7; i++) {
		handSelect.children[i].innerHTML = '<img crossorigin="Anonymous" src="../../assets/unselect.png">';
	}

	cubeGeo = new THREE.BoxGeometry(64, 64, 64);				

	var testCube = new THREE.Mesh(cubeGeo, materials[0]);
	setGridPosition(testCube, 1, 1, 1);
	scene.add(testCube);
	objects.push(testCube);	

	//grids
	var bottomGridHelper = new THREE.GridHelper(64 * boardSize, boardSize);
	scene.add(bottomGridHelper);
	bottomGridHelper.position.x = 480;
	bottomGridHelper.position.y = -960;
	bottomGridHelper.position.z = 480;

	var rightGridHelper = new THREE.GridHelper(64 * boardSize, boardSize);
	rightGridHelper.position.x = 960;
	rightGridHelper.position.y = -480;
	rightGridHelper.position.z = 480;
	rightGridHelper.rotateZ(Math.PI / 2);
	scene.add(rightGridHelper);

	var backGridHelper = new THREE.GridHelper(64 * boardSize, boardSize);
	backGridHelper.position.x = 480;
	backGridHelper.position.y = -480;
	backGridHelper.position.x = 480;
	backGridHelper.rotateX(Math.PI / 2);
	scene.add(backGridHelper);
	
	//planes behind grids	
	var geometry = new THREE.PlaneBufferGeometry(960, 960);
	geometry.rotateX( - Math.PI / 2);
	
	bottomPlane = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial( {color: 0xd0d0d0 }));
	scene.add(bottomPlane);
	bottomPlane.position.copy(bottomGridHelper.position);
	bottomPlane.position.y -= 1;

	rightPlane = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial({color: 0xd0d0d0} ));
	scene.add(rightPlane);
	rightPlane.position.copy(rightGridHelper.position);
	rightPlane.position.x += 1;
	rightPlane.rotateZ(Math.PI / 2);


	backPlane = new THREE.Mesh(geometry, new THREE.MeshBasicMaterial({color: 0xd0d0d0} ));
	scene.add(backPlane);
	backPlane.position.copy(backGridHelper.position);
	backPlane.position.z -= 1;
	backPlane.rotateX(Math.PI / 2);
	//plane.position.y = bottomGridHelper.position.y;
	//plane.position.z = bottomGridHelper.position.z;
	
	//objects.push(plane);
		

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
	document.addEventListener('keyup', onDocumentKeyUp, false);
	document.addEventListener('keydown', onDocumentKeyDown, false);
	document.addEventListener('wheel', onMouseWheel, false);
	
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

	var allCollidables = objects.concat(newPlay);	
	var intersects = raycaster.intersectObjects(allCollidables);

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
	switch(event.button) {
		case 0:
			if(controlsEnabled) {	
				var allCollidables = objects.concat(newPlay);	
				var intersects = raycaster.intersectObjects(allCollidables);
				if(intersects.length > 0) {
					var intersect = intersects[0];
					var voxel = new THREE.Mesh(cubeGeo, materials[5]);
					voxel.position.copy(intersect.point).add(intersect.face.normal);
					voxel.position.divideScalar(64).floor().multiplyScalar(64).addScalar(32);
					//check that position is in bounds
					var gridX = screenCoorToGridCoor(voxel.position.x);
					var gridY = screenCoorToGridCoor(voxel.position.y);
					var gridZ = screenCoorToGridCoor(voxel.position.z);
					console.log("Block coordinates: " + gridX + " " + gridY + " " + gridZ);
					if(!(gridX < 0 || gridX >= boardSize || gridY < 0 || gridY >= boardSize || gridZ < 0 || gridZ >= boardSize)) {
						//in bounds, add to screen
						scene.add(voxel);
						newPlay.push(voxel);
						//Add turquoise highlight
						var highlight = new THREE.Mesh(cubeGeo, new THREE.MeshBasicMaterial( {color: 0x00ccff, opacity: 0.5, transparent: true}));
						highlight.position.copy(voxel.position);
						scene.add(highlight);
						newPlayHighlights.push(highlight);				
					}
				}
			}
			break;
		case 2:
			if(controlsEnabled) {
				var intersects = raycaster.intersectObjects(newPlay);
				if(intersects.length > 0) {
					var intersect = intersects[0];
					var index = newPlay.indexOf(intersect.object);
					console.log(index);
					if(index != -1) {
						scene.remove(newPlayHighlights[index]);
						scene.remove(newPlay[index]);
						newPlayHighlights.splice(index, 1);
						newPlay.splice(index, 1);
					} else {
						index = newPlayHighlights.indexOf(intersect.object);
						console.log("NewPlayHighlights index: " + index);
						if(index != -1) {
							scene.remove(newPlayHighlights[index]);
							scene.remove(newPlay[index]);
							newPlayHighlights.splice(index, 1);
							newPlay.splice(index, 1);
						}
					}
					
				}		
			}
			break;
	}
}				

function onDocumentKeyUp(event) {
	switch(event.keyCode) {
		case 13: //enter
			break;
	}
}

function onDocumentKeyDown(event) {
	var newHandSelect = handIndex;
	switch(event.keyCode) {
		case 48: //numrow 0
		case 96: //numpad 0
			break;
		case 49: //numrow 1
		case 97: //numpad 1
			newHandSelect = 0;
			console.log(1);
			break;
		case 50: //numrow 2
		case 98: //numpad 2
			newHandSelect = 1;
			break;
		case 51: //numrow 3
		case 99: //numpad 3
			newHandSelect = 2;
			break;
		case 52: //numrow 4
		case 100: //numpad 4
			newHandSelect = 3;
			break;
		case 53: //numrow 5
		case 101: //numpad 5
			newHandSelect = 4;
			break;
		case 54: //numrow 6
		case 102: //numpad 6
			newHandSelect = 5;
			break;
		case 55: //numrow 7
		case 103: //numpad 7
			newHandSelect = 6;
			break;
		case 56: //numrow 8
		case 104: //numpad 8
			break;

		case 57: //numrow 9
		case 104: //numpad 9
			break;
	}
	setHandSelect(newHandSelect);
}

function onMouseWheel(event) {
	var newHandSelect = handIndex;
	if(event.deltaY > 0) {
		newHandSelect--;
		newHandSelect += 7;
		newHandSelect = newHandSelect % 7;
		console.log(newHandSelect);
		setHandSelect(newHandSelect);
	} else if (event.deltaY < 0) {
		newHandSelect++;
		newHandSelect %= 7;
		setHandSelect(newHandSelect);
	}
}

function setHandSelect(newHandSelect) {
	console.log(handIndex);
	handSelect.children[handIndex].innerHTML = '<img crossorigin="Anonymous" src="../../assets/unselect.png">';
	handIndex = newHandSelect;
	handSelect.children[handIndex].innerHTML = '<img crossorigin="Anonymous" src="../../assets/select.png">';
}

//Takes an integer as input. Y is inverted, so use gridCoorToScreenCoorY
function gridCoorToScreenCoor(x) {
	return (x * 64) + 32;
}

function gridCoorToScreenCoorY(y) {
	return -1 * gridCoorToScreenCoor(y);
}


function screenCoorToGridCoor(x) {
	return (Math.abs(x) - 32) / 64;
}

function setGridPosition(cube, x, y, z) {
	cube.position.x = gridCoorToScreenCoor(x);
	cube.position.y = gridCoorToScreenCoorY(x);
	cube.position.z = gridCoorToScreenCoor(x);
}

function initTextures() {
	for(i = 'a'; i <= 'z'; i = String.fromCharCode(i.charCodeAt() + 1)) {
		var fileName = '../../assets/';
		fileName += i;
		fileName += '.png';
		var texture = new THREE.TextureLoader().load(fileName);
		texture.magFilter = THREE.LinearMipMapLinearFilter;
		texture.minFilter = THREE.LinearMipMapLinearFilter;
		textures.push(texture);
	}
}

function initMaterials() {
	for(i = 0; i < 26; i++) {	
		var material = new THREE.MeshLambertMaterial({map: textures[i]});
		materials.push(material);
	}
}

//creates a cube with the proper texture
//@param c = character to be on cube
function createCube(c) {
	for(i = 0; i < 26; i++) {
		console.log(textures[i]);
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
