#include "FadeWidget.h"

void UFadeWidget::Fade(bool Fade)
{
    if(FadeIn)
    {
        if(Fade)
            PlayAnimation(FadeIn);
        else
            PlayAnimationReverse(FadeIn);
    }  
}

