<h1>Tools and Middleware Coursework</h1>
<br/>
<h2>The Hinge</h2>
After some reasearch, I was able to find out about <a href="http://bullet.googlecode.com/svn/trunk/Demos/ConstraintDemo/ConstraintDemo.cpp"> this page</a> which contained a number of examples for bullet, and most importantly some example of Hinges and springs. I tried to use it to get it to work on my code, however, in order to create a hinge constraint, I had to use the rigid_bodies of my objects. The problem was that while scene_nodes have a get_rigid_body() method, using it would always cause the game to crash, and I could not figure out why.<br/><br/>
In the end, to be able to get around that issue, I decided to create a copy of the add_shape() method which is used to create and add a scene_node to app_scene. This copy, that I called createNewObjectWithRigidBody, is in all ways similar to add_shape(), except that it also has a pointer towards the pointer of the rigid body as a parameter. Using it, I was able to retrieve the rigid body on creation, which I was then able to use in my constraint.<br/><br/>
Once I had those rigid bodies, creating the hinge itself was not a big issue. I simply had to reuse the code from the example, modify it with my rigid bodies and add it to the world to get it back. The one issue I had, is that the btDynamicsWorld object was stored in visual_scene.h without any way to access it from my main file. I had to create a get function in visual_scene, and store a pointer towards the world in my main file to be able to add my constraint to the world. I could also have created a method in visual_scene to create a constraint, but I felt that my way was clearer to read.<br/><br/>
After that, all that remained was to present a good example of a hinge. I decided that the best way to do this was to show two spheres linked together, that would have some velocity. The user would then be able to see how the hinge limits their movement.


<br/><br/>
<h2>The Swing</h2>



<br/><br/>
<h2>The Collision Detection</h2>


<br/><br/>
<h2>The Sound System</h2>


<br/><br/>
<a href="">Short Youtube Presentation</a>
