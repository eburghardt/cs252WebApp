/**
 * @author mrdoob / http://mrdoob.com/
 */

THREE.PointerLockControls = function ( camera, domElement ) {

	this.camera = camera;
	
	this.domElement = ( domElement !== undefined) ? domElement : document;

	this.enabled = true;

	this.movementSpeed = 1.0;
	this.lookSpeed = 0.002;	

	this.moveForward = false;
	this.moveBackward = false;
	this.moveLeft = false;
	this.moveRight = false;
	this.moveDown = false
	this.moveUp = false;


	camera.rotation.set( 0, 0, 0 );

	var pitchObject = new THREE.Object3D();
	pitchObject.add( camera );

	var yawObject = new THREE.Object3D();
	yawObject.position.y = 10;
	yawObject.add( pitchObject );

	var PI_2 = Math.PI / 2;

	this.onMouseMove = function ( event ) {

		if ( this.enabled === false ) return;

		var movementX = event.movementX || event.mozMovementX || event.webkitMovementX || 0;
		var movementY = event.movementY || event.mozMovementY || event.webkitMovementY || 0;

		yawObject.rotation.y -= movementX * this.lookSpeed;
		pitchObject.rotation.x -= movementY * this.lookSpeed;

		pitchObject.rotation.x = Math.max( - PI_2, Math.min( PI_2, pitchObject.rotation.x ) );

	};

	this.onKeyDown = function(event) {
		switch(event.keyCode) {
			case 38: //up
			case 87: //w
				this.moveForward = true;
				break;
			case 37: //left
			case 65: //a
				this.moveLeft = true;
				break;
			case 40: //down
			case 83: //s
				this.moveBackward = true;
				break;
			case 39: //right
			case 68: //d
				this.moveRight = true;
				break;
			case 32: //space
				this.moveUp = true;
				break;
			case 17: //ctrl
				this.moveDown = true;
				break;
		}
	};

	this.onKeyUp = function(event) {
		switch(event.keyCode) {
			case 38: //up
			case 87: //w
				this.moveForward = false;
				break;
			case 37: //left
			case 65: //a
				this.moveLeft = false;
				break;
			case 40: //down
			case 83: //s
				this.moveBackward = false;
				break;
			case 39: //right
			case 68: //d
				this.moveRight = false;
				break;
			case 32: //space
				this.moveUp = false;
				break;
			case 17: //ctrl
				this.moveDown = false;
				break;
		}
	};

	
	//document.addEventListener( 'mousemove', onMouseMove, false );

	this.enabled = false;

	this.getObject = function () {

		return yawObject;

	};

	this.getDirection = function() {

		// assumes the camera itself is not rotated

		var direction = new THREE.Vector3( 0, 0, - 1 );
		var rotation = new THREE.Euler( 0, 0, 0, "YXZ" );

		return function( v ) {

			rotation.set( pitchObject.rotation.x, yawObject.rotation.y, 0 );

			v.copy( direction ).applyEuler( rotation );

			return v;

		};

	};

	this.update = function( delta ) {
		if(this.enabled === false) {
			 return;
		}

		var deltaPos = delta * this.movementSpeed;


		if(this.moveForward || 0) this.camera.translateZ(-deltaPos);
		if(this.moveBackward)	this.camera.translateZ(deltaPos);
		if(this.moveLeft)	this.camera.translateX(-deltaPos);
		if(this.moveRight)	this.camera.translateX(deltaPos);
		if(this.moveUp)		this.camera.translateY(deltaPos);
		if(this.moveDown)	this.camera.translateY(-deltaPos);
		
	}
	
	this.dispose = function() {
		window.removeEventListener('keyup', _onKeyUp, false);
		window.removeEventListener('keydown', _onKeyDown, flase);
		document.removeEventListener( 'mousemove', onMouseMove, false );

	};


	var _onKeyDown = bind(this, this.onKeyDown);
	var _onKeyUp = bind(this, this.onKeyUp);
	var _onMouseMove = bind(this, this.onMouseMove);

	window.addEventListener('keydown', _onKeyDown, false);
	window.addEventListener('keyup', _onKeyUp, false);
	document.addEventListener('mousemove', _onMouseMove, false);	

	function bind(scope, fn) {
		return function() {
			fn.apply(scope, arguments);
		};
	}

};
