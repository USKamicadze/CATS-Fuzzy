
// ==UserScript==
// @name        CATS.console
// @namespace   CATS
// @description extensions for CATS console
// @include     https://imcs.dvfu.ru/cats/main.pl?*f=console
// @require     https://code.jquery.com/jquery-3.0.0.min.js
// @version     1
// @grant       none
// ==/UserScript==
(function(){
  var saveAs=saveAs||function(e){"use strict";if("undefined"==typeof navigator||!/MSIE [1-9]\./.test(navigator.userAgent)){var t=e.document,n=function(){return e.URL||e.webkitURL||e},o=t.createElementNS("http://www.w3.org/1999/xhtml","a"),r="download"in o,i=function(e){var t=new MouseEvent("click");e.dispatchEvent(t)},a=/Version\/[\d\.]+.*Safari/.test(navigator.userAgent),c=e.webkitRequestFileSystem,d=e.requestFileSystem||c||e.mozRequestFileSystem,u=function(t){(e.setImmediate||e.setTimeout)(function(){throw t},0)},s="application/octet-stream",f=0,l=4e4,v=function(e){var t=function(){"string"==typeof e?n().revokeObjectURL(e):e.remove()};setTimeout(t,l)},p=function(e,t,n){t=[].concat(t);for(var o=t.length;o--;){var r=e["on"+t[o]];if("function"==typeof r)try{r.call(e,n||e)}catch(i){u(i)}}},w=function(e){return/^\s*(?:text\/\S*|application\/xml|\S*\/\S*\+xml)\s*;.*charset\s*=\s*utf-8/i.test(e.type)?new Blob(["\uFEFF",e],{type:e.type}):e},y=function(t,u,l){l||(t=w(t));var y,m,S,h=this,R=t.type,O=!1,g=function(){p(h,"writestart progress write writeend".split(" "))},b=function(){if(m&&a&&"undefined"!=typeof FileReader){var o=new FileReader;return o.onloadend=function(){var e=o.result;m.location.href="data:attachment/file"+e.slice(e.search(/[,;]/)),h.readyState=h.DONE,g()},o.readAsDataURL(t),void(h.readyState=h.INIT)}if((O||!y)&&(y=n().createObjectURL(t)),m)m.location.href=y;else{var r=e.open(y,"_blank");void 0===r&&a&&(e.location.href=y)}h.readyState=h.DONE,g(),v(y)},E=function(e){return function(){return h.readyState!==h.DONE?e.apply(this,arguments):void 0}},N={create:!0,exclusive:!1};return h.readyState=h.INIT,u||(u="download"),r?(y=n().createObjectURL(t),void setTimeout(function(){o.href=y,o.download=u,i(o),g(),v(y),h.readyState=h.DONE})):(e.chrome&&R&&R!==s&&(S=t.slice||t.webkitSlice,t=S.call(t,0,t.size,s),O=!0),c&&"download"!==u&&(u+=".download"),(R===s||c)&&(m=e),d?(f+=t.size,void d(e.TEMPORARY,f,E(function(e){e.root.getDirectory("saved",N,E(function(e){var n=function(){e.getFile(u,N,E(function(e){e.createWriter(E(function(n){n.onwriteend=function(t){m.location.href=e.toURL(),h.readyState=h.DONE,p(h,"writeend",t),v(e)},n.onerror=function(){var e=n.error;e.code!==e.ABORT_ERR&&b()},"writestart progress write abort".split(" ").forEach(function(e){n["on"+e]=h["on"+e]}),n.write(t),h.abort=function(){n.abort(),h.readyState=h.DONE},h.readyState=h.WRITING}),b)}),b)};e.getFile(u,{create:!1},E(function(e){e.remove(),n()}),E(function(e){e.code===e.NOT_FOUND_ERR?n():b()}))}),b)}),b)):void b())},m=y.prototype,S=function(e,t,n){return new y(e,t,n)};return"undefined"!=typeof navigator&&navigator.msSaveOrOpenBlob?function(e,t,n){return n||(e=w(e)),navigator.msSaveOrOpenBlob(e,t||"download")}:(m.abort=function(){var e=this;e.readyState=e.DONE,p(e,"abort")},m.readyState=m.INIT=0,m.WRITING=1,m.DONE=2,m.error=m.onwritestart=m.onprogress=m.onwrite=m.onabort=m.onerror=m.onwriteend=null,S)}}("undefined"!=typeof self&&self||"undefined"!=typeof window&&window||this.content);"undefined"!=typeof module&&module.exports?module.exports.saveAs=saveAs:"undefined"!=typeof define&&null!==define&&null!==define.amd&&define([],function(){return saveAs});
  var frames_container = $('<div/>', {id:"mukhin_frames"}).appendTo($('.main_menu').parent());
  $('.main_menu').parent().append(
    $('<tr/>', {
     class: "sub_menu",
    }).append(
      $('<td/>', {
       colspan:2,
      }).append(
        $('<button/>', {
          id: 'mukhin_filter',
          text : "filter",
        }).click(function() {
          //var button = $(this);
          var frames_container = $('#mukhin_frames');
          var button = $('#mukhin_filter');
          $('a[href$="f=view_source"]').not('[title*="C++"]').parent().parent().slideUp();
          frames_container.empty();
          var get_status = function($a) {
            var f = $a.parent().find('font');
            if (f.length > 0) {
              return f.text();
            }
            return $a.parent().find('b').text();
          };
          $('a[href$="f=view_source"][title*="C++"]').each(function(){
            var $a = $(this);
            var status = '';
            $.each(get_status($a).split(' '), function(index, value) {
              var v = value.trim();
              if (v.length > 0) {
               status += v.charAt(0).toUpperCase();
              }
            });
            $a.data('status', status);
            console.log(status);
          }).appendTo(frames_container);
          frames_container.find('a').each(function(i) {
            var $a = $(this);
            var status = $a.data('status');
            var rid = $a.attr('href').match(/rid=(.*?);/)[1];
            setTimeout(function(){
              var $frame = $('<iframe/>', {
                id: 'frame_' + rid,
                src: $a.attr('href').replace(/f=view_source/, "f=download_source")
              });
              $a.append($frame);
              setTimeout(function () {
                var text =  $($('#frame_' + rid)[0].contentDocument.body).find('pre').text();
                var filename = 'CATS#solution#' + $a.text().replace(/\s/g, '_') + '#' + rid + '#' + status + '.cpp';
                var blob = new Blob([text], {type: "text/plain;charset=utf-8"});
                saveAs(blob, filename);
              }, 1000);
            }, 2000 * i);
          })
        })
      )
    )
  )
  
})();