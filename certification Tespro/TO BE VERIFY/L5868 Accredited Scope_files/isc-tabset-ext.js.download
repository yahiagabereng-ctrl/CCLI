function getIframeEleByHTMLPane(mainPane) {
	try {
		return document.getElementById(mainPane.$ru).firstChild;
	} catch (e) {

		return null;
	}
}

function cleanIframe(iframeEl) {

	var iframe = iframeEl.contentWindow;
	if (iframeEl) {
		iframeEl.src = 'about:blank';
		try {
			iframe.document.write('');
			iframe.document.clear();
		} catch (e) {
		}
		// 以上可以清除大部分的内存和文档节点记录数了
	}
	var _parentElement = iframeEl.parentNode;
	if (_parentElement) {
		_parentElement.removeChild(iframeEl);

	}
}
 

isc.defineClass("LasTabSet", "TabSet").addProperties({
	 closeTabIconSize:12,
	    paneContainerClassName:"tabSetContainer",
	    paneMargin:5,
	    pickerButtonSize:20,
	    pickerButtonSrc:"[SKIN]picker.png",
	    showScrollerRollOver:false,
	    scrollerButtonSize:19,
	    scrollerSrc:"[SKIN]scroll.png",
	    showEdges:false,
	    symmetricScroller:false,
	    symmetricPickerButton:false,
	    tabBarThickness:26,
	    defaultTabHeight:26,
	    useSimpleTabs:true
});
 

 