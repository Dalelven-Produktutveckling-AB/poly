/**
 * Copyright © 2021 Dalelven Produktutveckling AB
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the “Software”), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

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