/* test for constructor, prototype, inheritance (pure ES5) */

function assert(actual, expected, message) {
    function get_full_type(o) {
        var type = typeof(o);
        if (type === 'object') {
            if (o === null)
                return 'null';
            if (o.constructor && o.constructor.name)
                return o.constructor.name;
        }
        return type;
    }

    if (arguments.length == 1)
        expected = true;

    if (typeof actual === typeof expected) {
        if (actual === expected) {
            if (actual !== 0 || (1 / actual) === (1 / expected))
                return;
        }
        if (typeof actual === 'number') {
            if (isNaN(actual) && isNaN(expected))
                return true;
        }
        if (typeof actual === 'object') {
            if (actual !== null && expected !== null
            &&  actual.constructor === expected.constructor
            &&  actual.toString() === expected.toString())
                return;
        }
    }
    throw Error("assertion failed: got " +
                get_full_type(actual) + ":|" + actual + "|, expected " +
                get_full_type(expected) + ":|" + expected + "|" +
                (message ? " (" + message + ")" : ""));
}

/* Rectangle constructor */
function Rectangle(x, y) {
    this.x = x;
    this.y = y;
}

/* static method: return a closure that captures val */
Rectangle.getClosure = function(val) {
    return function() { return val; };
};

/* static method: call a function with an argument */
Rectangle.callFn = function(fn, arg) {
    return fn(arg);
};

/* FilledRectangle inherits from Rectangle */
function FilledRectangle(x, y, color) {
    Rectangle.apply(this, [x, y]);
    this.color = color;
}

FilledRectangle.prototype = Object.create(Rectangle.prototype);
FilledRectangle.prototype.constructor = FilledRectangle;

function cb(param)
{
    return "test" + param;
}

function test()
{
    var r1, r2, func;
    r1 = new Rectangle(100, 200);
    assert(r1.x, 100);
    assert(r1.y, 200);

    /* test inheritance */
    r2 = new FilledRectangle(100, 200, 0x123456);
    assert(r2.x, 100);
    assert(r2.y, 200);
    assert(r2.color, 0x123456);

    /* test instanceof chain */
    assert(r2 instanceof FilledRectangle);
    assert(r2 instanceof Rectangle);

    /* test closure */
    func = Rectangle.getClosure("abcd");
    assert(func(), "abcd");

    /* test function call */
    assert(Rectangle.callFn(cb, "abc"), "testabc");

    print("test_rect: PASS");
}

test();
