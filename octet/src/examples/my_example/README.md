<h1>Introduction to Games Programming Coursework</h1>
<br/>
<h2>Generating the terrain</h2>
Generating a basic Pool table was not a difficult task in itself. All it really is, is a large green box for the field, and 4 boxes for the walls. The difficulty was to be able to implement some holes on this table. I considered trying to find a pool table mesh online that would do the trick, however I ended up deciding to simply have the holes hover over the table.<br/><br/>
My reasoning was that I knew that the point of view of the player would always be fixed, and using hovering black discs instead of holes would have the effect required without requiring me to use someone else's art. In the end, while it did the trick there was something missing that made it look a bit strange. I figured that I could use a custom shader to make sure that those holes would appear to have some depth.<img src="http://www.jeanpascalevette.com/img/poolHole.PNG" alt="Pool Hole" align="right" /><br/><br/>
In order to do it, I passed a new variable to my fragment shader called height, which really was the radius of the hole. Using it in conjuction with the model_pos_, I could figure out how far away each fragment was from the center of the model, and I used that to determine the intensity of the color. The result is that while the holes are all black, the black is more intense near the center, and gradually gets lighter as you get closer to the edge, which gives it that feeling of depth.<br/>



<a href="https://www.youtube.com/watch?v=nL9_l3HN5ss&feature=youtu.be">Short Youtube presentation</a>