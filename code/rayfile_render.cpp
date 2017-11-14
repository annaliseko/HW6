#include "rayfile.h"
#include "image.h"
#include "camera.h"
#include "group.h"
#include "main.h"
#include "material.h"
#include "light.h"
#include "shape.h"
#include <math.h>
#include <vector>
#include <iostream>


/*
* method to cast rays through each pixel of image into the scene.
* uses getColor to find the color of an intersection point for each 
* pixel.  makes pretty pictures.
*/
void RayFile::raytrace (Image* image)
{

    // these will be useful
    int imageWidth = image->getWidth();
    int imageHeight = image->getHeight();
    
    // RAY_CASTING TODO
    // Read camera info and compute distance D to view window and the coordinates of its center and top left
    // Hint: check out camera object, which is part of RayFile
    // Hint: the half height angle is input in degrees but trig function require angles in radians
    // RAY_CASTING TODO
    
    double halfHeightDeg = getCamera()->getHalfHeightAngle();
    double halfHeight = (halfHeightDeg * M_PI)/180.0;
    double D = imageHeight/(2.0*tan(halfHeight));
    // std::cout << "distance is: " << D << std::endl;

    Point3d center = getCamera()->getPos() + D*((getCamera()->getDir()).normalize());
    Vector3d upVec = (imageHeight/2.0)*((getCamera()->getUp()).normalize());
    Vector3d leftVec = (imageWidth/2.0)*((getCamera()->getRight()).normalize());
    Point3d upperLeft = center + upVec - leftVec;

    // for printing progress to stderr...
    double nextMilestone = 1.0;

    //
    // ray trace the scene
    //

    for (int j = 0; j < imageHeight; ++j)
    {
        for (int i = 0; i<imageWidth; ++i)
        {

            // Compute the ray to trace
            Rayd theRay;

            // RAY_CASTING TODO
            // Compute and set the starting poisition and direction of the ray through pixel i,j
            // HINT: be sure to normalize the direction vector
            // RAY_CASTING TODO

            Point3d ij = upperLeft + (i+0.5)*((getCamera()->getRight()).normalize()) - (j+0.5)*((getCamera()->getUp()).normalize());

            std::cout << "Camera position: " << getCamera()->getPos() << std::endl;
            theRay.setPos(getCamera()->getPos());
            theRay.setDir(Vector3d(ij - getCamera()->getPos()));

            // get the color at the closest intersection point

            Color3d theColor = getColor(theRay, options->recursiveDepth); // Q: How much of the computation does getColor do, after we cast out the ray? Seems like it is doing almost everything

            // the image class doesn't know about color3d so we have to convert to pixel
            // update pixel
            Pixel p;

            p.r = theColor[0];
            p.g = theColor[1];
            p.b = theColor[2];

            image->setPixel(i, j, p);

        } // end for-i

        // update display
        // you don't need to touch this part!

        if (options->progressive)
        {
            display();
        }
        else if (!options->quiet)
        {
            if (((double) j / (double) imageHeight) <= nextMilestone)
            {
                cerr << "." << flush;
                nextMilestone -= (1.0 / 79.0);
            }
        }
    } // end for-j
}


/* 
* get the color of the scene with respect to theRay
*/


Color3d RayFile::getColor(Rayd theRay, int rDepth)
{
    // some useful constants
    Color3d white(1,1,1);
    Color3d black(0,0,0);

    // check for intersections
    Intersection intersectionInfo;
    intersectionInfo.theRay=theRay;


    double dist = root->intersect(intersectionInfo);

    if (dist <=EPSILON)
        return background;

    // intersection found so compute color
    Color3d color;

    // check for texture

    // add emissive term

    // add ambient term
    
    // add contribution from each light
    std::cout << "GETCOLOR ray: " << intersectionInfo.theRay << std::endl;
    for (VECTOR(Light*)::iterator theLight = lights.begin(); theLight != lights.end(); ++theLight)
    {
        std::cout << "i'm still alive" << std::endl;
        if (!((*theLight)->getShadow(intersectionInfo, root))) {
            color += (*theLight)->getDiffuse(intersectionInfo);
            color += (*theLight)->getSpecular(intersectionInfo);            
            //std::cout << "sup" << std::endl;

        }
        std::cout << "i'm the best" << std::endl;

    }
    std::cout << "i got here" << std::endl;

    // stop if no more recursion required
    if (rDepth == 0)
        return color; // done


    // stop if we are already at white
    color.clampTo(0,1);
    if (color==white) // can't add any more
        return color;

    // recursive step

    // reflection

    // transmission

    // compute transmitted ray using snell's law

    return color;
}

