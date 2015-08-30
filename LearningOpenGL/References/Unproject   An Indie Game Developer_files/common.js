/** JS Logics */
(function($){
	
	/** Drop Downs */
	function infinityMenu() {
		
		/** Superfish Menu */
		$( 'ul.sf-menu, .menu ul' ).supersubs({			
			
			minWidth: 16,
			maxWidth: 27,
			extraWidth: 1			
		
		}).superfish({		
			
			delay: 100,
			speed: 'fast',
			animation: { opacity:'show', height:'show' },
			dropShadows: false		
		
		});
		
		/** Last Child */
		$( 'ul.sf-menu li:last-child a, .menu ul li:last-child a' ).css( 'border-bottom', 'none' );
	
	}
	
	/** Equal Height Columns */
	function infinityEqualHeightCols() {
		
		var contentHeight = $( '#content' ).height();
		var sidebarHeight = $( '#sidebar' ).height();
		var highestCol = Math.max( contentHeight, sidebarHeight );
		
		if( contentHeight < highestCol ) {
			$( '#content' ).height( highestCol ); 
		}
	
	}
	
	/** jQuery Document Ready */
	$(document).ready(function(){
		infinityMenu();
	});
	
	/** jQuery Windows Load */
	$(window).load(function(){
		infinityEqualHeightCols();
	});

})(jQuery);