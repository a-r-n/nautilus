"use strict";

function assert(actual, expected, message) {
    if (arguments.length == 1)
        expected = true;

    if (actual === expected)
        return;

    if (actual !== null && expected !== null
    &&  typeof actual == 'object' && typeof expected == 'object'
    &&  actual.toString() === expected.toString())
        return;

    throw Error("assertion failed: got |" + actual + "|" +
                ", expected |" + expected + "|" +
                (message ? " (" + message + ")" : ""));
}

// load more elaborate version of assert if available
try { __loadScript("test_assert.js"); } catch(e) {}

/*----------------*/

function my_func(a, b)
{
    return a + b;
}

function test_function()
{
    function f(a, b) {
        var i, tab = [];
        tab.push(this);
        for(i = 0; i < arguments.length; i++)
            tab.push(arguments[i]);
        return tab;
    }
    function constructor1(a) {
        this.x = a;
    }
    
    var r, g;
    
    r = my_func.call(null, 1, 2);
    assert(r, 3, "call");

    r = my_func.apply(null, [1, 2]);
    assert(r, 3, "apply");

    r = new Function("a", "b", "return a + b;");
    assert(r(2,3), 5, "function");

    print("Debug print 0");
    g = f.bind(1, 2);
    print("Debug print 1");
    assert(g.length, 1);
    print("Debug print 2");
    assert(g.name, "bound f");
    print("Debug print 3");
    assert(g(3), [1,2,3]);
    print("Debug print 4");

    g = constructor1.bind(null, 1);
    r = new g();
    print("Debug print 5");
    assert(r.x, 1);

    print("Debug print 6");
}

test_function();
