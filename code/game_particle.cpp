
void UpdateParticles(game_state* State, render_group* Group, f32 Dt)
{
    f32 a = 3.0f;
    for(u32 Index=0;Index<4;++Index)
    {
        particle* Particle = State->Particles + State->NextParticle++;
        if(State->NextParticle >= NUM_PARTICLES)
        {
            State->NextParticle = 0;
        }
        
        f32 x = (((float)rand()/(float)(RAND_MAX)) * a) - a/2.0f;
        f32 y = (((float)rand()/(float)(RAND_MAX)) * a);
        
        Particle->P = V2(0,4);
        Particle->dP = V2(0,0);
        Particle->Life = 1.0f;
        
        Particle->ddP = V2(x, y*10.0f);
        
    }
    
    for(u32 Index=0;Index<NUM_PARTICLES;++Index)
    {
        particle* Particle = State->Particles + Index;
        
        Particle->dP = Particle->ddP * Dt + Particle->dP;
        Particle->P = Particle->P + Particle->dP * Dt + Particle->ddP * Square(Dt);
        
        PushCube(Group, V3(Particle->P.x, Particle->P.y, 0.0f), V3(0.5,0.5,0.5), V3(0.9,0.12,0.6));
    }
    
}

