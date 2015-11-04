<h1>Tools and Middleware Coursework</h1>
<br/>
<h2>The Hinge</h2>
After some reasearch, I was able to find out about <a href="http://bullet.googlecode.com/svn/trunk/Demos/ConstraintDemo/ConstraintDemo.cpp"> this page</a> which contained a number of examples for bullet, and most importantly some example of Hinges and springs. I tried to use it to get it to work on my code, however, in order to create a hinge constraint, I had to use the rigid_bodies of my objects. The problem was that while scene_nodes have a get_rigid_body() method, using it would always cause the game to crash, and I could not figure out why.<br/><br/>
In the end, to be able to get around that issue, I decided to create a copy of the add_shape() method which is used to create and add a scene_node to app_scene. This copy, that I called createNewObjectWithRigidBody, is in all ways similar to add_shape(), except that it also has a pointer towards the pointer of the rigid body as a parameter. Using it, I was able to retrieve the rigid body on creation, which I was then able to use in my constraint.<br/><br/>
Once I had those rigid bodies, creating the hinge itself was not a big issue. I simply had to reuse the code from the example, modify it with my rigid bodies and add it to the world to get it back. The one issue I had, is that the btDynamicsWorld object was stored in visual_scene.h without any way to access it from my main file. I had to create a get function in visual_scene, and store a pointer towards the world in my main file to be able to add my constraint to the world. I could also have created a method in visual_scene to create a constraint, but I felt that my way was clearer to read.<br/><br/>
<img src="http://www.jeanpascalevette.com/img/balls.PNG" alt="Balls" align="right"/>
After that, all that remained was to present a good example of a hinge. I decided that the best way to do this was to show two spheres linked together, that would have some velocity. The user would then be able to see how the hinge limits their movement.


<br/><br/>
<h2>The Swing</h2>
Implementing the swing for the most part reused concepts I had already figured out when trying the implement the hinge. I used the same webpage as an example, and then used the method I had already created to get my rigid bodies. From that point the difficulty was settings up all the small variables that affect the behaviour of the spring to make sure that it looked "springy" enough for the demo, such as the upper and lower limits, and the stiffness. In the end, I had two boxes side by side linked by a spring. A hinge constraint with a third box guarantees that only one of the boxes will move, and the moving box then exhibits a spring-like behaviour.
<img src="http://www.jeanpascalevette.com/img/boxes.PNG" alt="Boxes" align="right"/>


<br/><br/>
<h2>The Collision Detection</h2>
Once again, the same webpage was again the source I used to learn how to implement the collision detection system (<a href="http://bullet.googlecode.com/svn/trunk/Demos/CollisionInterfaceDemo/CollisionInterfaceDemo.cpp"> this page</a>), along with <a href="http://hamelot.co.uk/programming/using-bullet-only-for-collision-detection/">another page</a> that provided similar information. The collision detection is run on every frame and checks every possible point of contact for every rigid body. Upon detecting the collision, I am outputing a confirmation in the Console.<br/><br/>
In addition, I wanted to implement a sound effect when there is a collision. I will talk more about implementing FMOD below, however I want to mention that one of the issues I faced was that there was multiple collisions between rigid bodies and the ground, and that the spring constraint was causing multiple collisions a second.
<br/><br/>
In order to fix those two issues, I decided first to add a new field to scene_node called "ignoresCollisions", which is used to set a body to ignore collisions. This is set to true for the ground, which is then used to ignore the ground's collision events.
<br/><br/>
While I don't mind having multiple collision events when an object stays in contact with another for more than a frame, I wanted to make sure that the sound file for the collision would not play multiple times. In order to do this, I would is the isPlaying() method from FMOD to make sure that the sound file was finished before running it again.


<br/><br/>
<h2>The Loading System</h2>
Loading the different object based on settings from an XML file was not a big problem, as it is something I had already done for the Intro to Games Programming coursework. What was more interesting however, is that I wanted to be able to create constraints based on the data from the XML file. In the end, I decided that on top of the list of shapes, I would have a List of Links, which would contain the list of all my Spring and Hinge constraints. The structure of those links was simply the type of Link (Hinge or Spring), the id of the two shapes linked (based on the order on which they are in the xml file), and the Axis affected for the Hinge link. If I wanted to create more complicated Spring constraints I would have needed to set some extra properties in the XML file (Upper and Lower limits, stiffness,...), however it was not necessary for my demo.<br/><br/>
While I was reading my file, I would store all the data recorded in vectors of predef_shape and predef_link. Those are two data classes that I created to hold the properties retrieved from the XML file.<br/><br/>
Once the data has been retrieved, the shapes are created in the world based on the settings stored in the vector. Following that, each link is created based on the created shapes and the data stored in the vector of predef_link.

<br/><br/>
<h2>The Sound System</h2>


<br/><br/>
<a href="">Short Youtube Presentation</a>
