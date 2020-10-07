import { Component, Input, OnInit } from '@angular/core';

@Component({
  selector: 'app-post-list',
  templateUrl: './post-list.component.html',
  styleUrls: ['./post-list.component.scss']
})
export class PostListComponent implements OnInit {

  @Input() title: String;
  @Input() content: String;
  @Input() loveIts: number;
  created_at: any;

  constructor() { }

  ngOnInit(): void {
    this.created_at = new Date();
  }

  onLike() {
    this.loveIts += 1;
    console.log(this.loveIts);
  }

  getTextColor() {
    if(this.loveIts > 0)
      return 'green';
    else if(this.loveIts < 0)
      return 'red';
    else
      return 'black'
  }

  onDislike() {
    this.loveIts -= 1;
    console.log(this.loveIts);
  }
}
