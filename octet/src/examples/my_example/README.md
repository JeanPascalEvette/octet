<h1>Introduction to Games Programming Coursework</h1>
<br/>
<h2>Generating the terrain</h2>
Generating a basic Pool table was not a difficult task in itself. All it really is, is a large green box for the field, and 4 boxes for the walls. The difficulty was to be able to implement some holes on this table. I considered trying to find a pool table mesh online that would do the trick, however I ended up deciding to simply have the holes hover over the table.<br/><br/>
My reasoning was that I knew that the point of view of the player would always be fixed, and using hovering black discs instead of holes would have the effect required without requiring me to use someone else's art. In the end, while it did the trick there was something missing that made it look a bit strange. I figured that I could use a custom shader to make sure that those holes would appear to have some depth.<img src="http://www.jeanpascalevette.com/img/poolHole.PNG" alt="Pool Hole" align="right" /><br/><br/>
In order to do it, I passed a new variable to my fragment shader called height, which really was the radius of the hole. Using it in conjuction with the model_pos_, I could figure out how far away each fragment was from the center of the model, and I used that to determine the intensity of the color. The result is that while the holes are all black, the black is more intense near the center, and gradually gets lighter as you get closer to the edge, which gives it that feeling of depth.<br/>


<br/>
<br/>
<h2>Generating the levels</h2>
In order to generate a level, I only require 3 pieces of data. The number of attempts allowed, the initial position and velocity of the white ball, and the initial position and velocity of the red balls. While the levels I have currently created do not make use of an initial velocity for any ball, it is included in the XML file structure in order to have it as an option if I want to create a level that uses that feature. As I have said, I am using XML files to store the level's informations as it is a very readable format that makes the level creation process quite easy. On top of that, the code to read from an XML file using tinyxml2 is more readable and maintainable than the code to read from a csv file. <br/><br/>
Switching between the levels is fairly straight forward; every XML file is nammed LevelX.xml (X being the number of the current level), and whenever I detect that the red balls have all been destroyed or that the cheat to skip the level has been used, I remove every ball, increment the variable storing the number of the current level, and load a new file using that variable in the name. Whenever the program is unable to find a file with that name, then it means that the player has completed every single available level, and I display a congratulatory message.<img src="http://www.jeanpascalevette.com/img/LevelFiles.PNG" alt="Level Files" align="right" /><br/><br/>

<br/><br/>
<h2>The gameplay</h2>
At its root, the gameplay is quite basic. It simply makes use of octet's bullet physics to let the user have fun with the collision of objects. There are however a number of small gameplay elements that were quite complex to implement but that improve the feel of the game greatly.<br/><br/>

<h3>The Drag-and-shoot</h3>
There are multiple way to represent the action of using a cue to shoot a ball. The one that felt the most natural to me was to select the ball with the mouse then drag back in a movement similar to pulling back the cue, before releasing to shoot the ball. In order to implement this, I had to figure out a way to handle mouse events. While I had found out how to handle keyboard events, I couldn't find a function to handle mouse events no matter what keyword I would use. I finally realised that octet considers mouse events to be similar to keyboard events and that I could simply use the key_lmb enum for the is_key_down() function. Once I figured that out, it was fairly straight forward to use get_mouse_pos to retrieve the mouse's screen coordinates. I could then form a vector between the mouse's initial coordinate and the mouse's release coordinate to figure out what velocity my white ball would get.
<br/><br/>
One Issue I did face is that I was having trouble converting the screen coordinates of the mouse position into world coordinates. There is a function to do this in the camera_instance, however it was not giving me the results I expected. I ended up writing the position of each click in the console, and used that to figure out manually how to convert those coordinates.
<br/><br/>
When I got that working, I wanted to make sure that the player would have an indication that the the dragging event was being registered. The best way to do that was to draw a line in between the white ball and wherever the mouse was. In order to do this, I decided to simply delete the line at each frame, and redraw it if the mouse was still being pressed. This meant that the line would always follow the movement of the mouse.

<br/><br/>
<h3>The Pockets</h3>

<br/><br/>
<h3>Keeping objects Awake</h3>

<br/><br/>
<a href="https://www.youtube.com/watch?v=nL9_l3HN5ss&feature=youtu.be">Short Youtube presentation</a>