#pragma once

namespace poly
{
template<class BeginIter, class EndIter, class OutIter>
OutIter copy(BeginIter first, EndIter last, OutIter out)
{
    while(first != last)
    {
        *out = *first;
        ++out;
        ++first;
    }
    return out;
}

template<class BeginIter, class EndIter, class OutIter, class UnaryPredicate>
OutIter copy_if(BeginIter first, EndIter last, OutIter out, UnaryPredicate pred)
{
    while(first != last)
    {
        if(pred(*first))
        {
            *out = *first;
        }
        ++out;
        ++first;
    }
    return out;
}

template<class BeginIter, class EndIter, class OutIter, class UnaryPredicate>
OutIter copy_while(BeginIter first, EndIter last, OutIter out, UnaryPredicate pred)
{
    while(first != last)
    {
        if(pred(*first))
        {
            *out = *first;
            ++out;
            ++first;
        }
        else
        {
            break;
        }
    }
    return out;
}

template<class BeginIter, class EndIter, class OutIter, class UnaryPredicate>
OutIter copy_until(BeginIter first, EndIter last, OutIter out, UnaryPredicate pred)
{
    while(first != last)
    {
        if(!pred(*first))
        {
            *out = *first;
            ++out;
            ++first;
        }
        else
        {
            break;
        }
    }
    return out;
}
}