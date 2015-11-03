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
<a href="https://www.youtube.com/watch?v=nL9_l3HN5ss&feature=youtu.be">Short Youtube presentation</a>