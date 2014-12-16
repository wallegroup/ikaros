/******************************************************************************
 Copyright (c) 2008-2012 Ryan Juckett
 http://www.ryanjuckett.com/
 
 This software is provided 'as-is', without any express or implied
 warranty. In no event will the authors be held liable for any damages
 arising from the use of this software.
 
 Permission is granted to anyone to use this software for any purpose,
 including commercial applications, and to alter it and redistribute it
 freely, subject to the following restrictions:
 
 1. The origin of this software must not be misrepresented; you must not
 claim that you wrote the original software. If you use this software
 in a product, an acknowledgment in the product documentation would be
 appreciated but is not required.
 
 2. Altered source versions must be plainly marked as such, and must not be
 misrepresented as being the original software.
 
 3. This notice may not be removed or altered from any source
 distribution.
 ******************************************************************************/

//******************************************************************************
// CalcDampedSimpleHarmonicMotion
// This function will update the supplied position and velocity values over
// the given time according to the spring parameters.
// - An angular frequency is given to control how fast the spring oscillates.
// - A damping ratio is given to control how fast the motion decays.
//     damping ratio > 1: over damped
//     damping ratio = 1: critically damped
//     damping ratio < 1: under damped
//******************************************************************************

#include <cmath>

void CalcDampedSimpleHarmonicMotion(
                                    float *pPos,            // position value to update
                                    float *pVel,            // velocity value to update
                                    float equilibriumPos,   // position to approach
                                    float deltaTime,        // time to update over
                                    float angularFrequency, // angular frequency of motion
                                    float dampingRatio      // damping ratio of motion
) {
    const float epsilon = 0.0001f;
    
    // if there is no angular frequency, the spring will not move
    if (angularFrequency < epsilon)
        return;
    
    // clamp the damping ratio in legal range
    if (dampingRatio < 0.0f)
        dampingRatio = 0.0f;
    
    // calculate initial state in equilibrium relative space
    const float initialPos = *pPos - equilibriumPos;
    const float initialVel = *pVel;
    
    // if over-damped
    if (dampingRatio > 1.0f + epsilon)
    {
        // calculate constants based on motion parameters
        // Note: These could be cached off between multiple calls using the same
        //       parameters for deltaTime, angularFrequency and dampingRatio.
        const float maxDampingRatio = 2.0;
        const float deltaTimeMax = 1.0;
        const float initialVelMax = 1.0;
        const float initialPosMax = 330.0;
        
        const float za          = -angularFrequency * dampingRatio;
        const float zaMax       = -angularFrequency * maxDampingRatio;
        
        const float zb          = angularFrequency * sqrtf(dampingRatio*dampingRatio - 1.0f);
        const float zbMax       = angularFrequency * sqrtf(maxDampingRatio*maxDampingRatio - 1.0f);
        
        const float z1          = za - zb;
        const float z1Max       = zaMax - zbMax;
        
        const float z2          = za + zb;
        const float z2Max       = zaMax + zbMax;
        
        const float expTerm1    = expf( z1 * deltaTime );
        const float expTerm1Max = expf( z1Max * deltaTimeMax );
        
        const float expTerm2    = expf( z2 * deltaTime );
        const float expTerm2Max = expf( z2Max * deltaTimeMax );
        
        // update motion
        float c1                = (initialVel - initialPos * z2) / (-2.0f * zb); // z1 - z2 = -2*zb
        float c1Max             = (initialVelMax - initialPosMax * z2Max) / (-2.0f * zbMax); // z1 - z2 = -2*zb
        
        float c2                = initialPos - c1;
        float c2Max             = initialPosMax - c1Max;
        
        *pPos = equilibriumPos + c1*expTerm1 + c2*expTerm2;
        // Hack to make this number be sort of between 0 and 1
        *pVel = fabs( ( (c1 * z1 * expTerm1) + (c2 * z2 * expTerm2) ) / ( (c1Max * z1Max * expTerm1Max) + (c2Max * z2Max * expTerm2Max) ) ) + 0.25f * (1.0f - deltaTime);
    }
    // else if critically damped
    else if (dampingRatio > 1.0f - epsilon)
    {
        // calculate constants based on motion parameters
        // Note: These could be cached off between multiple calls using the same
        //       parameters for deltaTime, angularFrequency and dampingRatio.
        
        // deltaTime is max 1.0
        // initialVel is max 1.0
        // initialPos is max 330
        // angularFrequency is constant
        
        // this is max expf(-angularFrequency) = 1.0
        const float expTerm = expf( -angularFrequency * deltaTime );
        const float expTermMax = expf( -angularFrequency );
        
        // this is max 1.0 + angularFrequency + 330
        float c1 = initialVel + angularFrequency * initialPos;
        float c1max = 1.0f + angularFrequency * 330;
        
        // this is max 330
        float c2 = initialPos;
        
        // this is max ( (1.0+angularFrequency+330) * 1.0 + 330 ) * expf(-angularFrequency) )
        float c3 = (c1 * deltaTime + c2) * expTerm;
        float c3max = ( c1max * 1.0f + 330 ) *  expTermMax;
        
        *pPos = equilibriumPos + c3;
        
        // Hack to make this number be sort of between 0 and 1
        *pVel = fabs( ( (c1 * expTerm) - (c3 * angularFrequency) ) / ( (c1max * expTermMax) - (c3max * angularFrequency) ) ) + 0.25f * (1.0f - deltaTime);
        //*pVel = fabs( ( (c1 * expTerm) - (c3 * angularFrequency) ) / ( (c1max * expTermMax) - (c3max * angularFrequency) ) );
    }
    // else under-damped
    else
    {
        // calculate constants based on motion parameters
        // Note: These could be cached off between multiple calls using the same
        //       parameters for deltaTime, angularFrequency and dampingRatio.
        const float omegaZeta = angularFrequency * dampingRatio;
        const float alpha = angularFrequency * sqrt(1.0f - dampingRatio*dampingRatio);
        const float expTerm = expf( -omegaZeta * deltaTime );
        const float cosTerm = cosf( alpha * deltaTime );
        const float sinTerm = sinf( alpha * deltaTime );
        
        // update motion
        float c1 = initialPos;
        float c2 = (initialVel + omegaZeta*initialPos) / alpha;
        *pPos = equilibriumPos + expTerm*(c1*cosTerm + c2*sinTerm);
        // Hack to make this number be sort of between 0 and 1
        *pVel = fabs( (-expTerm*( (c1*omegaZeta - c2*alpha)*cosTerm +
                                 (c1*alpha + c2*omegaZeta)*sinTerm)) / 50);
    }
}