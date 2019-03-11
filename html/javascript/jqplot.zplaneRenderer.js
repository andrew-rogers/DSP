/**
 * jqPlot
 * Pure JavaScript plotting plugin using jQuery
 *
 * Version: 1.0.8
 * Revision: 1250
 *
 * Copyright (c) 2009-2013 Chris Leonello
 * jqPlot is currently available for use in all personal or commercial projects 
 * under both the MIT (http://www.opensource.org/licenses/mit-license.php) and GPL 
 * version 2.0 (http://www.gnu.org/licenses/gpl-2.0.html) licenses. This means that you can 
 * choose the license that best suits your project and use it accordingly. 
 *
 * Although not required, the author would appreciate an email letting him 
 * know of any substantial use of jqPlot.  You can reach the author at: 
 * chris at jqplot dot com or see http://www.jqplot.com/info.php .
 *
 * If you are feeling kind and generous, consider supporting the project by
 * making a donation at: http://www.jqplot.com/donate.php .
 *
 * sprintf functions contained in jqplot.sprintf.js by Ash Searle:
 *
 *     version 2007.04.27
 *     author Ash Searle
 *     http://hexmen.com/blog/2007/03/printf-sprintf/
 *     http://hexmen.com/js/sprintf.js
 *     The author (Ash Searle) has placed this code in the public domain:
 *     "This code is unrestricted: you are free to use it however you like."
 * 
 */
(function($) {
    /**
     * Class: $.jqplot.ZplaneRenderer
     * jqPlot Plugin to draw zero-pole plots.
     *
     * This source code is derived from jqplot.ohlcRenderer.js and modified
     * by Andrew Rogers
     * 
     * To use this plugin, include the renderer js file in 
     * your source:
     * 
     * > <script type="text/javascript" src="plugins/jqplot.zplaneRenderer.js"></script>
     * 
     * Then you set the renderer in the series options on your plot:
     * 
     * > series: [{renderer:$.jqplot.ZplaneRenderer}]
     * 
     * For Zplane plots, data should be specified
     * like so:
     * 
     * > dat = [[real,imag], [real,imag], ...]
     * 
     */
    $.jqplot.ZplaneRenderer = function(){
        // subclass line renderer to make use of some of its methods.
        $.jqplot.LineRenderer.call(this);
        // prop: candleStick
        // true to render chart as candleStick.
        // Must have an open price, cannot be a hlc chart.
        this.candleStick = false;
        // prop: tickLength
        // length of the line in pixels indicating open and close price.
        // Default will auto calculate based on plot width and 
        // number of points displayed.
        this.tickLength = 'auto';
        // prop: bodyWidth
        // width of the candlestick body in pixels.  Default will auto calculate
        // based on plot width and number of candlesticks displayed.
        this.bodyWidth = 'auto';
        // prop: openColor
        // color of the open price tick mark.  Default is series color.
        this.openColor = null;
        // prop: closeColor
        // color of the close price tick mark.  Default is series color.
        this.closeColor = null;
        // prop: wickColor
        // color of the hi-lo line thorugh the candlestick body.
        // Default is the series color.
        this.wickColor = null;
        // prop: fillUpBody
        // true to render an "up" day (close price greater than open price)
        // with a filled candlestick body.
        this.fillUpBody = false;
        // prop: fillDownBody
        // true to render a "down" day (close price lower than open price)
        // with a filled candlestick body.
        this.fillDownBody = true;
        // prop: upBodyColor
        // Color of candlestick body of an "up" day.  Default is series color.
        this.upBodyColor = null;
        // prop: downBodyColor
        // Color of candlestick body on a "down" day.  Default is series color.
        this.downBodyColor = null;
        // prop: hlc
        // true if is a hi-low-close chart (no open price).
        // This is determined automatically from the series data.
        this.hlc = false;
        // prop: lineWidth
        // Width of the hi-low line and open/close ticks.
        // Must be set in the rendererOptions for the series.
        this.lineWidth = 1.5;
        this._tickLength;
        this._bodyWidth;
    };
    
    $.jqplot.ZplaneRenderer.prototype = new $.jqplot.LineRenderer();
    $.jqplot.ZplaneRenderer.prototype.constructor = $.jqplot.ZplaneRenderer;
    
    // called with scope of series.
    $.jqplot.ZplaneRenderer.prototype.init = function(options) {
        options = options || {};
        // lineWidth has to be set on the series, changes in renderer
        // constructor have no effect.  set the default here
        // if no renderer option for lineWidth is specified.
        this.lineWidth = options.lineWidth || 1.5;
        $.jqplot.LineRenderer.prototype.init.call(this, options);
        this._type = 'zplane';    
    };

    // called with scope of plot.
    $.jqplot.ZplaneRenderer.checkOptions = function(target, data, options) {
        
	//alert("preInit()");
        

	// if ZplaneRenderer chosen then setup some options
	var setopts = false;
        if(options.seriesDefaults){
          if (options.seriesDefaults.renderer == $.jqplot.ZplaneRenderer) {
              setopts = true;
          }
          else if (options.series) {
            for (var i=0; i < options.series.length; i++) {
                if (options.series[i].renderer == $.jqplot.ZplaneRenderer) {
                    setopts = true;
                }
            }
          }
	}
        if (setopts) {
	    if(!options.axesDefaults)options.axesDefaults={};
	    options.grid = options.grid || {};
	    options.grid.renderer = $.jqplot.ZplaneGridRenderer;
            options.series=[
              {
		    showLine : false,
                    markerOptions: { style:"circle" }
              },
              {
		    showLine : false,
                    markerOptions: { style:"x" }
              }
            ];
	}
        

	
    };


    /**
     * Class: $.jqplot.ZplaneGridRenderer
     *
     *  Referenced jqplot.pyramidGridRenderer.js to create this work although little
     *  resemblence remains, Andrew Rogers.
     */

    $.jqplot.ZplaneGridRenderer = function() {
        $.jqplot.CanvasGridRenderer.call(this);
    };
    
    $.jqplot.ZplaneGridRenderer.prototype = new $.jqplot.CanvasGridRenderer();
    $.jqplot.ZplaneGridRenderer.prototype.constructor = $.jqplot.ZplaneGridRenderer;
        
    
    // called with scope of grid object.
    $.jqplot.ZplaneGridRenderer.prototype.init = function(options){
       $.jqplot.CanvasGridRenderer.prototype.init.call(this, options);     
    }; 

    $.jqplot.ZplaneGridRenderer.prototype.draw = function(){
       this._ctx = this._elem.get(0).getContext("2d");
       var ctx = this._ctx;
       var axes = this._axes;
       $.jqplot.CanvasGridRenderer.prototype.draw.call(this); 
       ctx.save();
       ctx.translate(axes['xaxis'].u2p(0),axes['yaxis'].u2p(0));
       var xscale=axes['xaxis'].u2p(1)-axes['xaxis'].u2p(0);
       var yscale=axes['yaxis'].u2p(1)-axes['yaxis'].u2p(0);
       ctx.scale(xscale,yscale);
       ctx.beginPath();
       ctx.arc(0,0,1,0,2*Math.PI,0);
       ctx.closePath();
       ctx.restore();
       ctx.stroke();
    }; 

    $.jqplot.preInitHooks.push($.jqplot.ZplaneRenderer.checkOptions);

})(jQuery);    
