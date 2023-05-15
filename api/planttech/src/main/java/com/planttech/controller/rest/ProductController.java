package com.planttech.controller.rest;

import java.util.List;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.DeleteMapping;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PathVariable;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.PutMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.planttech.domain.Product;
import com.planttech.domain.Page;
import com.planttech.service.ProductService;

import io.swagger.v3.oas.annotations.Operation;
import io.swagger.v3.oas.annotations.Parameter;
import io.swagger.v3.oas.annotations.enums.ParameterIn;
import io.swagger.v3.oas.annotations.responses.ApiResponse;
import io.swagger.v3.oas.annotations.responses.ApiResponses;
import io.swagger.v3.oas.annotations.tags.Tag;

@Tag(name = "Product", description = "식물 경매/거래")
@RestController
@RequestMapping("/product")
public class ProductController {
	
	@Autowired ProductService productService;
	
	
	@Operation(summary = "식물 거래글 조회", description = "키워드, 카테고리, 페이지 번호, 페이지당 글 수를 이용해 거래글을 조회합니다.")
	@GetMapping()
	public List<Product> getArticleList(Page page) {
		System.out.println("::: getArticleList :::");
		System.out.println(page.toString());
		
		return productService.getArticleList(page);
	}
	
	@Operation(summary = "식물 거래글 상품 상세 조회", description = "조회한 거래글의 상품 고유번호를 이용해 해당상품의 상세 정보를 조회합니다.")
	@GetMapping("/{productId}")
	public int getArticleProductInfo(@PathVariable("productId") int prodcutId) {
		System.out.println("::: getArticleProductInfo :::");
		System.out.println(prodcutId);
		
		return prodcutId;
	}
	
	@Operation(summary = "식물 거래글 추가", description = "식물 거래글을 올립니다. 객체 값중 생성일자 및 수정일자는 받지 않습니다.")
	@PostMapping()
	public int addArticle(@Parameter(description = "거래 글 내용", in = ParameterIn.HEADER) @RequestBody  Product article) {
		System.out.println("::: addArticle :::"); 
		System.out.println(article.toString()); 
		
		return productService.addArticle(article);
	}
	
	@Operation(summary = "식물 거래글 수정", description = "식물 거래글을 수정합니다. 파라미터 값은 거래글 추가와 같으며 마찬가지로 객체 값중 생성일자 및 수정일자는 받지 않습니다.")
	@PutMapping()
	public int modifyArticle(@RequestBody Product article) {
		System.out.println("::: updateArticle :::"); 
		System.out.println(article.toString()); 
		
		return productService.modifyArticle(article);
	}
	
	
	@Operation(summary = "식물 거래글 삭제", description = "식물 거래글을 삭제합니다. 객체 값중 생성일자 및 수정일자는 받지 않습니다.")
	@DeleteMapping()
	public int removeArticle(@RequestParam int articleNo) {
		System.out.println("::: removeArticle :::"); 
		Product article = new Product();
		
		article.setProductNo(articleNo);
		System.out.println(article.toString()); 
		
		return productService.removeArticle(article);
	}
	
	

	@Operation(summary = "식물 경매: 즉시 구매", description = "식물 거래글에 즉시 구매 요청을 진행합니다.")
	@PostMapping("/buy")
	public int addProductBuy(@RequestBody  Product article) {
		System.out.println("::: addProductBuy :::"); 
		System.out.println(article.toString()); 
		
		return -1;
	}
	
	@Operation(summary = "식물 경매: 입찰가 제시", description = "식물 거래글에 입찰가를 제시합니다.")
	@PostMapping("/bid")
	public int addProductBid(@RequestBody  Product article) {
		System.out.println("::: addProductBid :::"); 
		System.out.println(article.toString()); 
		
		return -1;
	}
	
	
}
