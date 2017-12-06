if(!Detector.webgl) {
	Detector.addGetWebGLMessage();
	document.getElementByID('container').innerHTML = "";
}
var container;
var camera, controls, scene, renderer;
var raycaster;
var controlsEnabled;

//Raycasted pointer
var rollOverMesh, rollOverMaterial;

//Geometry for all objects
var cubeGeo, cubeMaterial;

//For controls	
var clock = new THREE.Clock();

//Keeping track of objects
var objects = [];

//All possible textures and materials
var textures = [];
var materials = [];

//Board size constant
var boardSize = 15;


//keeping track of hand information
var hand;
var handchars = "abcdefg";
var handIndex = 0;

var handSelect;

//Keeping track of current play
var newPlay = [];
var newPlayHighlights = [];

//Websocket
var ws;

var threeTexture = new THREE.TextureLoader().load('../../assets/three.png');
threeTexture.minFilter = THREE.LinearMipMapLinearFilter;
threeTexture.magFilter = THREE.LinearMipMapLinearFilter;
var threeMesh = new THREE.MeshLambertMaterial({map: threeTexture});

var numTiles = 1470;

var port;

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

	getPort("http://165.227.181.230/port", initializePort);	
	
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

	//place multipliers
	placeMultipliers();

	var startCubeGeo = new THREE.BoxGeometry(30, 30, 30);
	startCubeMesh = new THREE.Mesh(startCubeGeo, rollOverMaterial);
	setGridPosition(startCubeMesh, 7, 7, 7);
	scene.add(startCubeMesh);	
	objects.push(startCubeMesh);

	//cubes
	initTextures();
	initMaterials();	
	/*
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
	*/
	drawHand();
	cubeGeo = new THREE.BoxGeometry(64, 64, 64);				

	/*
	var testCube = new THREE.Mesh(cubeGeo, materials[0]);
	setGridPosition(testCube, 1, 1, 1);
	scene.add(testCube);
	objects.push(testCube);	

	var w = new THREE.Mesh(cubeGeo, materials[22]);
	var o = new THREE.Mesh(cubeGeo, materials[14]);
	var r = new THREE.Mesh(cubeGeo, materials[17]);
	var d = new THREE.Mesh(cubeGeo, materials[3]);
	var tres = new THREE.Mesh(cubeGeo, threeMesh);
	var d2 = new THREE.Mesh(cubeGeo, materials[3]);

	setGridPosition(w, 11, 7, 5);
	setGridPosition(o, 11, 7, 6);
	setGridPosition(r, 11, 7, 7);
	setGridPosition(d, 11, 7, 8);
	setGridPosition(tres, 11, 6, 8);
	setGridPosition(d2, 12, 6, 8);

	scene.add(w);
	scene.add(o);
	scene.add(r);
	scene.add(d);
	scene.add(tres);
	scene.add(d2);
	*/
	
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

	renderer = new THREE.WebGLRenderer({antialias: false});
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
	//console.log(raycaster.ray.direction);

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
				if(handIndex < 0) return;
	
				var allCollidables = objects.concat(newPlay);	
				var intersects = raycaster.intersectObjects(allCollidables);
				if(intersects.length > 0) {
					var intersect = intersects[0];
					var voxel = new THREE.Mesh(cubeGeo, materials[getSelectedHandCharCode()]);
					voxel.position.copy(intersect.point).add(intersect.face.normal);
					voxel.position.divideScalar(64).floor().multiplyScalar(64).addScalar(32);
					//check that position is in bounds
					var gridX = screenCoorToGridCoor(voxel.position.x);
					var gridY = screenCoorToGridCoorY(voxel.position.y);
					var gridZ = screenCoorToGridCoor(voxel.position.z);
					console.log("Block coordinates: " + gridX + " " + gridY + " " + gridZ);
					if(!(gridX < 0 || gridX >= boardSize || gridY < 0 || gridY >= boardSize || gridZ < 0 || gridZ >= boardSize)) {
						//in bounds, add to screen
						voxel.character = getSelectedHandChar();
						console.log(voxel.character);
						console.log("Before: " + handchars)
						handchars = handchars.slice(0, handIndex) + handchars.slice(handIndex + 1);
						console.log("After: " + handchars)
						drawHand();
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
						addToHand(newPlay[index].character);					
						scene.remove(newPlay[index]);
						newPlayHighlights.splice(index, 1);
						newPlay.splice(index, 1);
						drawHand();
					} else {
						index = newPlayHighlights.indexOf(intersect.object);
						console.log("NewPlayHighlights index: " + index);
						if(index != -1) {
							scene.remove(newPlayHighlights[index]);
							addToHand(newPlay[index].character);
							scene.remove(newPlay[index]);
							newPlayHighlights.splice(index, 1);
							newPlay.splice(index, 1);
						}
						drawHand();
					}
					
				}		
			}
			break;
	}
}				

function onDocumentKeyUp(event) {
	switch(event.keyCode) {
		case 13: //enter
			sendPlay();
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
		newHandSelect += handchars.length;
		newHandSelect = newHandSelect % handchars.length;
		console.log(newHandSelect);
		setHandSelect(newHandSelect);
	} else if (event.deltaY < 0) {
		newHandSelect++;
		newHandSelect %= handchars.length;
		setHandSelect(newHandSelect);
	}
}

function setHandSelect(newHandSelect) {
	console.log(handIndex);
	if(handIndex < 0)	return;
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
	return (x - 32) / 64;
}

function screenCoorToGridCoorY(y) {
	return -(y + 32) / 64;
}

function setGridPosition(cube, x, y, z) {
	cube.position.x = gridCoorToScreenCoor(x);
	cube.position.y = gridCoorToScreenCoorY(y);
	cube.position.z = gridCoorToScreenCoor(z);
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
//@param x y z = position
function createCube(c, x, y, z) {
	var cube = new THREE.Mesh(cubeGeo, materials[c.charCodeAt() - 'a'.charCodeAt()]);
	setGridPosition(cube, x, y, z);
	objects.push(cube);
	scene.add(cube);
}

//Hand index will be -1 if no characters are in hand.
function drawHand() {
	//handchars = "cdfgabc";

	for(i = 0; i < handchars.length; i++) {
		hand.children[i].innerHTML = textures[handchars[i].charCodeAt() - 'a'.charCodeAt()].image.outerHTML;
	}

	for(i = handchars.length; i < 7; i++) {
		hand.children[i].innerHTML = '<img crossorigin="Anonymous" src="../../assets/empty.png">';
	}

	if(handchars.length > 0) {
		handIndex = 0;
		handSelect.children[0].innerHTML = '<img crossorigin="Anonymous" src="../../assets/select.png">';
		for(i = 1; i < 7; i++) {
			handSelect.children[i].innerHTML = '<img crossorigin="Anonymous" src="../../assets/unselect.png">';
		}
	} else {
		handIndex = -1;
		for(i = 0; i < 7; i++) {
			handSelect.children[i].innerHTML = '<img crossorigin="Anonymous" src="../../assets/unselect.png">';
		}

	}

}

function addToHand(c) {
	if(handchars.length >= 7)	return;

	handchars += c;
}

//Does nothing if hand length isn't correct
function setHand(string) {
	if(string.length == 7) {
		handchars = string;
		drawHand();
	}
}

function getSelectedHandCharCode() {
	return handchars[handIndex].charCodeAt() - 'a'.charCodeAt();
}

function getSelectedHandChar() {
	return handchars[handIndex];
}

function sendPlay() {
	//do nothing if no characters are actually played
	if(handchars.length == 7) {
		ws.send("play:");
		return;
	}

	//check that all placed blocks are colinear
	//if only one character is place, it's good
	if(handchars.length < 6) {
		//we have at least 2 characters on the board. They all must share at least 2 coordinates with eachother
		var firstx = newPlay[0].position.x;
		var firsty = newPlay[0].position.y;
		var firstz = newPlay[0].position.z;
	
		var secondx = newPlay[1].position.x;
		var secondy = newPlay[1].position.y;
		var secondz = newPlay[1].position.z;

		if(firstx == secondx && firsty == secondy && firstz == secondz) {
			illegalPlay("Two blocks in same place");
			return; //two blocks in the same place
		}

		if(firstx == secondx && firsty == secondy) {
			//for each remaining element in new play, make sure they have the appropriate x and y
			for(i = 2; i < newPlay.length; i++) {
				if(newPlay[i].position.x != firstx || newPlay[i].position.y != firsty) return;
			}
			//Copy newPlay so we can sort it
			var newPlayCopy = newPlay.slice();
			newPlayCopy.sort(function(a, b) {return a.position.z - b.position.z});		
	
			var string = "";
			var lastpos = -1;
			for(i = 0; i < newPlay.length; i++) {
				if(lastpos != -1) {
					if(screenCoorToGridCoor(newPlayCopy[i].position.z) == lastpos) {
						illegalPlay("Two blocks in same place");
						return;
					}
					//insert spaces if there was a gap between this character and the last
					for(j = 1; j < screenCoorToGridCoor(newPlayCopy[i].position.z) - lastpos; j++) {
						string += " ";
					} 
					
				}
				string += newPlayCopy[i].character;
				lastpos = screenCoorToGridCoor(newPlayCopy[i].position.z);
			}
			console.log(string);		

			ws.send("play:" + string + ":" + screenCoorToGridCoor(newPlayCopy[0].position.x) + ":" + screenCoorToGridCoorY(newPlayCopy[0].position.y) + ":" + screenCoorToGridCoor(newPlayCopy[0].position.z) + ":2\n");
		} else if(firstx == secondx && firstz == secondz) {
			//for each remaining element in new play, make sure they have the appropriate x and z
			for(i = 2; i < newPlay.length; i++) {
				if(newPlay[i].position.x != firstx || newPlay[i].position.z != firstz) return;
			}
			
			//Copy newPlay so we can sort it
			var newPlayCopy = newPlay.slice();
			//because y increase negatively on the screen we sort it backwards
			newPlayCopy.sort(function(a, b) {return b.position.y - a.position.y});		
	
			var string = "";
			var lastpos = -1;
			for(i = 0; i < newPlay.length; i++) {
				if(lastpos != -1) {
					if(screenCoorToGridCoor(newPlayCopy[i].position.y) == lastpos) {
						illegalPlay("Two blocks in same place");
						return;
					}

					//insert spaces if there was a gap between this character and the last
					for(j = 1; j < lastpos - screenCoorToGridCoor(newPlayCopy[i].position.y); j++) {
						string += " ";
					} 
					
				}
				string += newPlayCopy[i].character;
				lastpos = screenCoorToGridCoor(newPlayCopy[i].position.y);
			}
			console.log(string);	

			ws.send("play:" + string + ":" + screenCoorToGridCoor(newPlayCopy[0].position.x) + ":" + screenCoorToGridCoorY(newPlayCopy[0].position.y) + ":" + screenCoorToGridCoor(newPlayCopy[0].position.z) + ":1\n");
		} else if (firsty == secondy && firstz == secondz){
			//for each remaining element in new play, make sure they have the appropriate y and z
			for(i = 2; i < newPlay.length; i++) {
				if(newPlay[i].position.y != firsty || newPlay[i].position.z != firstz) return;
			}
			
			//Copy newPlay so we can sort it
			var newPlayCopy = newPlay.slice();
			newPlayCopy.sort(function(a, b) {return a.position.x - b.position.x});		
	
			var string = "";
			var lastpos = -1;
			for(i = 0; i < newPlay.length; i++) {
				if(lastpos != -1) {
					if(screenCoorToGridCoor(newPlayCopy[i].position.x) == lastpos) {
						illegalPlay("Two blocks in same place");
						return;
					}

					//insert spaces if there was a gap between this character and the last
					for(j = 1; j < screenCoorToGridCoor(newPlayCopy[i].position.x) - lastpos; j++) {
						string += " ";
					} 
					
				}
				string += newPlayCopy[i].character;
				lastpos = screenCoorToGridCoor(newPlayCopy[i].position.x);
			}
			console.log(string);	
			ws.send("play:" + string + ":" + screenCoorToGridCoor(newPlayCopy[0].position.x) + ":" + screenCoorToGridCoorY(newPlayCopy[0].position.y) + ":" + screenCoorToGridCoor(newPlayCopy[0].position.z) + ":0\n");
		}
	} else {
		ws.send("play:" + " " + newPlay[0].character + " " + ":" +  screenCoorToGridCoor(newPlay[0].position.x) + ":" + screenCoorToGridCoorY(newPlay[0].position.y) + ":" + screenCoorToGridCoor(newPlay[0].position.z) + ":0\n"); 
		ws.send("play:" + " " + newPlay[0].character + " " + ":" +  screenCoorToGridCoor(newPlay[0].position.x) + ":" + screenCoorToGridCoorY(newPlay[0].position.y) + ":" + screenCoorToGridCoor(newPlay[0].position.z) + ":1\n"); 
		ws.send("play:" + " " + newPlay[0].character + " " + ":" +  screenCoorToGridCoor(newPlay[0].position.x) + ":" + screenCoorToGridCoorY(newPlay[0].position.y) + ":" + screenCoorToGridCoor(newPlay[0].position.z) + ":2\n"); 
	}
}


function parseMessageType(message) {
	console.log(typeof message);
	console.log(message.indexOf("play") !== -1);
	if(message.indexOf("play") !== -1) {
		//message format: "play:<string>:x:y:z:direction"
		clearNewPlay();
		playString(message.slice(5));

	} else if(message.indexOf("scores") !== -1) {
		//message format: "scores;playername: score;playername: score...\n"
		setScores(message.slice(7));
	} else if(message.indexOf("turn") !== -1) {
		//message format: "turn:true/false:playername\n"
		//true indicates it's your turn
		setTurnIndicator(message.slice(5));
	} else if(message.indexOf("tiles") !== -1) {
		//message format: "tiles:<numtilesremaining>\n"
		numTiles = message.slice(6);	
		setRemainingTilesIndicator(numTiles);	
	} else if(message.indexOf("hand") !== -1) {
		//message format: "hand:<7 characters>"
		//console.log("New hand: " + message.substring(5, 12));
		setHand(message.slice(5));	
	} else if(message.indexOf("denied") !== -1) {
		//message format: "denied:reason\n
		console.log("Reason: " + message.slice(7));
		illegalPlay(message.slice(7));
	} else if(message.indexOf("board") !== -1) {
		//message format: "board:"
		
	}
}

function illegalPlay(reason) {
	var denied = document.getElementById('denied');
	var deniedtext = document.getElementById('deniedtext');
	deniedtext.innerHTML = "Denied: " + reason;
	denied.style.display = '';

	setTimeout(function(){denied.style.display = 'none'}, 1000);
}

function setScores(scores) {
	var scoresText = document.getElementById('scorestext');
	var allScores = scores.split(";");
	var newText = "";
	for(i = 0; i < allScores.length; i++) {
		newText += allScores[i];
		newText += "<br><br>";
	}
	console.log(newText);
	scoresText.innerHTML = newText;
}

function setTurnIndicator(turnMessage) {
	var turnsText = document.getElementById('turnstext');
	if(turnMessage.indexOf("true") !== -1) {
		turnsText.innerHTML = "Your turn.";
	} else {
		turnsText.innerHTML = turnMessage.slice(turnMessage.indexOf(":") + 1) + "'s turn";
	}
}

function setRemainingTilesIndicator(numTiles) {
	var tilesText = document.getElementById('tilestext');
	tilesText.innerHTML = "Remaining tiles: " + numTiles;
}

function clearNewPlay() {
	for(i = 0; i < newPlay.length; i++) {
		handchars += newPlay[i].character;
		scene.remove(newPlay[i]);
		scene.remove(newPlayHighlights[i]);	
	}
	newPlay = [];
	newPlayHighlights = [];
	drawHand();
}

function playString(string){
	var parts = string.split(":");
	var play = parts[0];
	var x = parseInt(parts[1]);
	var y = parseInt(parts[2]);
	var z = parseInt(parts[3]);
	var dir = parseInt(parts[4]);

	console.log(play + " " + x + " "+ y + " "+ z + " " + dir);

	switch(dir) {
		case 0:
			for(i = 0; i < play.length; i++) {
				if(play[i].charCodeAt() >= 'a'.charCodeAt() && play[i].charCodeAt() <= 'z'.charCodeAt()) {
					createCube(play[i], x + i, y, z);					
				}
			}
			break;
		case 1:
			for(i = 0; i < play.length; i++) {

				if(play[i].charCodeAt() >= 'a'.charCodeAt() && play[i].charCodeAt() <= 'z'.charCodeAt()) {
					console.log("here");
					createCube(play[i], x, y + i, z);					
				}
			}
			break;
		case 2:
			for(i = 0; i < play.length; i++) {
				if(play[i].charCodeAt() >= 'a'.charCodeAt() && play[i].charCodeAt() <= 'z'.charCodeAt()) {
					createCube(play[i], x, y, z + i);					
				}
			}

			break;
	}


//	playStringRecursive(play, x, y, z, dir);	
}

function getPort(theUrl, callback) {
	var xmlHttp = new XMLHttpRequest();
	xmlHttp.onreadystatechange = function() {
		if(xmlHttp.readyState == 4 && xmlHttp.status == 200)
			callback(xmlHttp.responseText);
	}
	xmlHttp.open("GET", "https://cors-anywhere.herokuapp.com/" + theUrl, true); //true for asynchronous
	xmlHttp.send(null);
}


function initializePort(response) {
	console.log(response);

	console.log("ws://165.227.181.230:" + response);
	
	//Open websocket connection
	ws = new WebSocket("ws://165.227.181.230:" + response);

	//Websocket functions
	ws.onopen = function() {
		ws.send("Connected");
	};

	ws.onmessage = function(event) {
		var received = event.data;
		console.log(received);

		parseMessageType(received);
	
	};

}

function placeMultipliers() {
	var multiplierCubeGeo = new THREE.BoxGeometry(5, 5, 5);
	
	x2Material = new THREE.MeshBasicMaterial( {color: 0x0000aa, opacity: 0.5, transparent: true});
	
	x3Material = new THREE.MeshBasicMaterial( {color: 0xaa00aa, opacity: 0.5, transparent: true});
	x4Material = new THREE.MeshBasicMaterial( {color: 0xccaa00, opacity: 0.5, transparent: true});
	x5Material = new THREE.MeshBasicMaterial( {color: 0x00dda0, opacity: 0.5, transparent: true});

	var newCube;
	for(i = 0; i < boardSize; i++) {
		for(j = 0; j < boardSize; j++) {
			for(k = 0; k < boardSize; k++) {
				var distance = Math.abs(7 - i) + Math.abs(7 - j) + Math.abs(7 - k);
				switch(distance) {
					case 3:
						newCube = new THREE.Mesh(multiplierCubeGeo, x2Material);
						setGridPosition(newCube, i, j, k);
						scene.add(newCube);
						break;
					case 7:
						newCube = new THREE.Mesh(multiplierCubeGeo, x3Material);
						setGridPosition(newCube, i, j, k);
						scene.add(newCube);					
						break;
					case 12:
						newCube = new THREE.Mesh(multiplierCubeGeo, x4Material);
						setGridPosition(newCube, i, j, k);
						scene.add(newCube);					
						break;
					case 18:
						newCube = new THREE.Mesh(multiplierCubeGeo, x5Material);
						setGridPosition(newCube, i, j, k);
						scene.add(newCube);					
						break;

				}
			}
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
